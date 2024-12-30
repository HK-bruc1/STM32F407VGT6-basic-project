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
//这个页边界也是固定的？还是动态计算的？
//inner_addr是不是要校验？
void W25Q64_writePage(u32 inner_addr, u8* data, u16 length){
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





//W25Q64测试函数
void W25Q64_Test(void){
    u8 writeData[256] = {0}; // 初始化为0
    u8 readData[256] = {0};  // 读取数据缓冲区
    
    // 初始化W25Q64控制器
    W25Q64_Init();
    printf("W25Q64 Test Start...\r\n");
    
    // 填充测试数据
    for (int i = 0; i < 256; i++) {
        writeData[i] = i; // 填充0-255的数据
    }

    // 打印writeData数组的前16个元素
    printf("WriteData: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", writeData[i]);
    }
    printf("\r\n");
		//擦除扇区数据(ok)
    W25Q64_SectorErase(0x000000);
		//擦除块区数据(ok)
    //W25Q64_BlockErase(0x000000);
		//擦除芯片数据(ok)
    //W25Q64_ChipErase();
    // 写入数据到0x000000地址 写入一页数据
    //W25Q64_writePage(0x000000, writeData, 256);
    printf("Write 256 Bytes Data\r\n");
    
    // 从0x000000地址读取数据
    W25Q64_ReadData(0x000000, readData, 256);
    printf("Read 256 Bytes Data\r\n");
    
    // 打印readData数组的前16个元素
    printf("ReadData: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", readData[i]);
    }
    printf("\r\n");
    
    // 比较写入和读取的数据
    int errorCount = 0;
    for (int i = 0; i < 256; i++) {
        if (writeData[i] != readData[i]) {
            errorCount++;
        }
    }
    
    if (errorCount == 0) {
        printf("Data Verify Success!\r\n");
    } else {
        printf("Data Verify Failed! Error Count: %d\r\n", errorCount);
    }
}