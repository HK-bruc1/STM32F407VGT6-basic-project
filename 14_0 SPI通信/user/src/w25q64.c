#include "w25q64.h"
#include "stm32f4xx.h"                  // Device header
#include "spi.h"
#include "selfprintf.h"
#include "BEEP.h"

//W25Q64控制器初始化
void W25Q64_Init(void){
	//SPI初始化
		spi1_controller_init();
	
	//单独片选线配置，方便移植
		//配置PB14为片选引脚（通用输出模式）
		RCC->AHB1ENR |= (1U<<1);//使能GPIOA和GPIOB时钟
		GPIOB->MODER &= ~(3U<<28);  // 清除PB14原配置
		GPIOB->MODER |= (1U<<28);   // 配置为通用输出模式
		GPIOB->OTYPER &= ~(1U<<14); // 推挽输出
		GPIOB->OSPEEDR &= ~(3U<<28);// 清除速度配置
		GPIOB->OSPEEDR |= (1U<<29); // 设置为50MHz
		GPIOB->PUPDR &= ~(3U<<28);//无上下拉
		// 确保CS引脚初始化为高
		GPIOB->ODR |= (1U<<14);  // 拉高片选，通信时才拉低。
	
	//解除所有块写保护(防止之前有写保护),把状态寄存器全部清零
		W25Q64_WriteStatus(0x00);
}


// 写使能
void W25Q64_WriteEnable(void) {
		//片选拉低
    W25Q64_CS_LOW;
    spi1_controller_TransmitReceive(W25Q64_WRITE_ENABLE); // 发送写使能指令
		//片选拉高
    W25Q64_CS_HIGH;
}


// 读取状态寄存器
u8 W25Q64_ReadStatus(void) {
    u8 status;
    W25Q64_CS_LOW;
    spi1_controller_TransmitReceive(W25Q64_READ_STATUS); 
		//随便发一个数据，用于触发从机发数据
    status = spi1_controller_TransmitReceive(0xFF);      
    W25Q64_CS_HIGH;
    return status;
}

//写状态寄存器
void W25Q64_WriteStatus(u8 data){
	W25Q64_WriteEnable();
	W25Q64_CS_LOW;
	//先发送一个指令
	spi1_controller_TransmitReceive(W25Q64_WRITE_STATUS);
	//再写状态寄存器
	//只能写SRP,TB,BP2,BP1,BP0,其他位不变
	spi1_controller_TransmitReceive(data);
	W25Q64_CS_HIGH;
	
	
	//该指令执行完后，BUSY和WEL自动变为0，可以读取BUSY状态判断是否写入
	//不停的读取状态
	while(W25Q64_ReadStatus() & (1U<<0));
	
} 


//从指定地址读取数据(连续读)
//0x 7F F F FF ,127号块  15号扇区  15号页 255号字节处
void W25Q64_ReadData(u32 inner_addr, u8* data, u32 length) {
    W25Q64_CS_LOW;
    spi1_controller_TransmitReceive(W25Q64_READ_DATA); // 读数据指令
		//发送24位地址也只能8位8位的发
    spi1_controller_TransmitReceive((inner_addr >> 16) & 0xFF); // 地址高8位
    spi1_controller_TransmitReceive((inner_addr >> 8) & 0xFF);  // 地址中8位
    spi1_controller_TransmitReceive(inner_addr & 0xFF);         // 地址低8位
		//内部地址会自动递增
    for (u32 i = 0; i < length; i++) {
				//使用数据下标语法糖
        data[i] = spi1_controller_TransmitReceive(0xFF); // 读取数据
    }
    W25Q64_CS_HIGH;
}


//页编程,做多写256个字节
//这个页边界也是固定的
//inner_addr要校验
void W25Q64_writePage(u32 inner_addr, u8* data, u16 length){
		if (length == 0) {
			printf("写入长度为0，无效操作\r\n");
			return;
		}

	// 检查是否跨页
    u32 start_page = inner_addr / 256;
	//起始地址也使用了需要减去1
    u32 end_addr = inner_addr + length - 1;
    u32 end_page = end_addr / 256;
  //C语言中整数除法会自动舍去小数部分,这个特性刚好可以用来计算页号
	//这就像把一串连续的地址按照256个一组分页:
	//0~255 都除以256得到0(第0页)
	//256~511 都除以256得到1(第1页)
    if(start_page != end_page){
        printf("地址和元素长度冲突，不能跨页操作\r\n");
        return;
    }
		
	W25Q64_WriteEnable();
	//擦除扇区,单一职责，只负责写入，在外面擦除
	//W25Q64_SectorErase(inner_addr);
	W25Q64_CS_LOW;
	spi1_controller_TransmitReceive(W25Q64_PAGE_PROGRAM); 
	//发送24位地址也只能8位8位的发
	spi1_controller_TransmitReceive((inner_addr >> 16) & 0xFF); // 地址高8位
	spi1_controller_TransmitReceive((inner_addr >> 8) & 0xFF);  // 地址中8位
	spi1_controller_TransmitReceive(inner_addr & 0xFF);         // 地址低8位
	//printf("开始写入数据......\r\n");
	//内部地址会自动递增
	for (u16 i = 0; i < length; i++) {
			//使用数据下标语法糖
			spi1_controller_TransmitReceive(data[i]); // 写入数据
	}
	W25Q64_CS_HIGH;
	// 等待写入完成
    while(W25Q64_ReadStatus() & (1U<<0));
	//printf("写入成功......\r\n");
}


//连续跨页写操作
void W25Q64_skip_page_write(u32 inner_addr, u8* data, u32 length){
    if (length == 0) {
        printf("写入长度为0，无效操作\r\n");
        return;
    }

	  //已经写入的字节数
    u32 written = 0;
	 	//从0开始使用<即可
    while (written < length) {
			// 计算当前页可写入的字节数
			u16 page_remaining = 256 - (inner_addr % 256);
			//确定本次写入的具体字节数
			//剩余元素<本页剩余空间  或者  写满本页
			u16 to_write = (length - written < page_remaining) ? length - written : page_remaining;

			// 写入当前页
			W25Q64_writePage(inner_addr, data + written, to_write);

			// 更新已写入字节数和地址
			written += to_write;
			inner_addr += to_write;
     }
}



//扇区擦除操作，最小擦除范围了，写入之前需要擦除数据，全部变成1
//0x 7F F F FF ,127号块  15号扇区  15号页 255号字节处
void W25Q64_SectorErase(u32 inner_addr) {
		//先写使能，才能写入输入数据
    W25Q64_WriteEnable();
    W25Q64_CS_LOW;
		//发一个擦除指令
    spi1_controller_TransmitReceive(W25Q64_SECTOR_ERASE); 
		//发送24位地址也只能8位8位的发
		spi1_controller_TransmitReceive((inner_addr >> 16) & 0xFF); // 地址高8位
		spi1_controller_TransmitReceive((inner_addr >> 8) & 0xFF);  // 地址中8位
		spi1_controller_TransmitReceive(inner_addr & 0xFF);         // 地址低8位
    W25Q64_CS_HIGH;
		//不停的读取状态
		while(W25Q64_ReadStatus() & (1U<<0));
		//printf("扇区擦除完成\r\n");
}


//块区擦除
void W25Q64_BlockErase(u32 inner_addr){
	//先写使能，才能写入输入数据
  W25Q64_WriteEnable();
	W25Q64_CS_LOW;
	//发一个擦除指令
	spi1_controller_TransmitReceive(W25Q64_BLOCK_ERASE); 
	//发送24位地址也只能8位8位的发
	spi1_controller_TransmitReceive((inner_addr >> 16) & 0xFF); // 地址高8位
	spi1_controller_TransmitReceive((inner_addr >> 8) & 0xFF);  // 地址中8位
	spi1_controller_TransmitReceive(inner_addr & 0xFF);         // 地址低8位
	W25Q64_CS_HIGH;
	//不停的读取状态
	while(W25Q64_ReadStatus() & (1U<<0));
	//printf("块区擦除完成\r\n");
}


//多块连续擦除
void W25Q64_BlocksErase(u32 inner_addr,u8 blocks){
	while(blocks){
		W25Q64_BlockErase(inner_addr);
		//一块的大小为65536个字节
		inner_addr+=65536;
		blocks--;
	}
}


//芯片擦除 20s,不需要传入地址
void W25Q64_ChipErase(){
	//先写使能，才能写入输入数据
  W25Q64_WriteEnable();
	W25Q64_CS_LOW;
	//发一个擦除指令
	spi1_controller_TransmitReceive(W25Q64_CHIP_ERASE); 
	W25Q64_CS_HIGH;
	//不停的读取状态
	while(W25Q64_ReadStatus() & (1U<<0));
	//printf("芯片擦除完成\r\n");
}
