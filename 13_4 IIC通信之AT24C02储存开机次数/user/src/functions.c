#include "functions.h"
#include "adc.h"
#include "GeneralTim.h"
#include "wang_time.h"
#include "selfprintf.h"
#include "nvic.h"
#include "key.h"
#include "RNG.h"
#include "event_handler.h"
#include "at24c02.h"
#include "string.h"


//此源文件用于定义综合性函数，不定义各外设的初始化配置，调用时都是配置好的


volatile uint8_t randomColorFlag = 0;




// 记录并打印开机次数的函数
// 功能：通过AT24C02 EEPROM记录和追踪系统启动次数
void PrintBootCount(void) {
    // 常量定义，便于后期维护
    const u8 BOOT_FLAG = 0xAA;    //首次开机后写入的标志
    const u8 FLAG_ADDR = 255;     // 标志位地址（EEPROM最后一个字节）
    const u8 COUNT_ADDR = 0;      // 计数值地址（EEPROM第一个字节）
    
    u8 bootCount = 0;      
    u8 firstBootFlag = 0;
    
    // 读取首次开机标志，先判断读取数据没有问题后，再判断是否为BOOT_FLAG
	//firstBootFlag在读取后会被覆盖
    if (at24c02_read_byte(FLAG_ADDR, &firstBootFlag) == 0 && firstBootFlag != BOOT_FLAG) {
        // 首次开机，初始化
        bootCount = 1;
        at24c02_write_byte(FLAG_ADDR, BOOT_FLAG);    // 写入开机标志
        at24c02_write_byte(COUNT_ADDR, bootCount);   // 写入初始开机次数
    } else {
        // 非首次开机，读取并累加开机次数
        if (at24c02_read_byte(COUNT_ADDR, &bootCount) == 0) {
			//读取成功后，bootCount会被覆盖
            bootCount++;  // 直接累加
            at24c02_write_byte(COUNT_ADDR, bootCount);  // 写回新的开机次数
        } else {
            // 读取失败，重置为1
            bootCount = 1;
            at24c02_write_byte(COUNT_ADDR, bootCount);
        }
    }
    
    printf("开机次数: %d\n", bootCount);
}



//AT24C02存入结构体
void WriteBookToEEPROM(Book *book, u16 baseAddr) {
    // 使用跨页写入函数直接写入整个结构体
    at24c02_skip_page_write(baseAddr, (char*)book, sizeof(Book));
}


// 从AT24C02读取Book结构体
void ReadBookFromEEPROM(Book *book, uint16_t baseAddr) {
    // 使用连续读取函数直接读取整个结构体
    at24c02_page_read(baseAddr, (char*)book, sizeof(Book));
}

// 打印Book结构体内容
void PrintBook(const char* prefix, const Book* book) {
    printf("\n%s书籍信息:\n", prefix);
    printf("---------------------------\n");
    printf("书名: %s\n", book->name);
    printf("作者: %s\n", book->writer);
    printf("编号: %s\n", book->number);
    printf("点击量: %lu\n", (unsigned long)book->hot);
    printf("收藏数: %lu\n", (unsigned long)book->sc);
    printf("价格: %.2f\n", book->price);
    printf("---------------------------\n");
}


// 测试函数
void TestBookStorage(void) {
    // 1. 创建并打印原始数据
    Book BK1 = {"西游记","吴承恩","W201955",0,30,55.5};
    PrintBook("原始", &BK1);
    
    // 2. 写入EEPROM
    WriteBookToEEPROM(&BK1, 0);
    printf("数据已写入EEPROM\n");
    
    // 3. 从EEPROM读取
    Book readBook = {0};  // 清零初始化
    ReadBookFromEEPROM(&readBook, 0);
    
    // 4. 打印读取的数据
    PrintBook("读取", &readBook);
    
    // 5. 验证数据是否一致
    if (memcmp(&BK1, &readBook, sizeof(Book)) == 0) {
        printf("数据验证成功: 读取的数据与原始数据完全一致!\n");
    } else {
        printf("数据验证失败: 读取的数据与原始数据不一致!\n");
        
        // 详细比对各字段
        if (strcmp((char*)BK1.name, (char*)readBook.name) != 0) 
            printf("书名不一致\n");
        if (strcmp((char*)BK1.writer, (char*)readBook.writer) != 0) 
            printf("作者不一致\n");
        if (strcmp((char*)BK1.number, (char*)readBook.number) != 0) 
            printf("编号不一致\n");
        if (BK1.hot != readBook.hot) 
            printf("点击量不一致\n");
        if (BK1.sc != readBook.sc) 
            printf("收藏数不一致\n");
        if (BK1.price != readBook.price) 
            printf("价格不一致\n");
    }
}




//按键控制RGB颜色,先初始化tim5_RGBInit还有keyInit();
void tim5_Key_RGB(void){
	u8 keyVal = 0;
	keyVal = key_scan();
	switch(keyVal){
		//蓝色
		case 1:
			TIM5->CCR2 = 0;
			TIM5->CCR3 = 0;
			TIM5->CCR4 = 999;
			break;
		//绿色
		case 2:
			TIM5->CCR2 = 0;
			TIM5->CCR3 = 999;
			TIM5->CCR4 = 0;
			break;
		//红色
		case 3:
			TIM5->CCR2 = 999;
			TIM5->CCR3 = 0;
			TIM5->CCR4 = 0;
			break;
		case 4:
			//开启随机变色模式
			randomColorFlag = !randomColorFlag;
			if(!randomColorFlag) {
				TIM5->CCR2 = 0; // 停止随机变色时，可以选择一个默认颜色或关闭LED
				TIM5->CCR3 = 0;
				TIM5->CCR4 = 0;
      }
			break;
		default:
			break;
	}
}

//RGB随机变色函数
void changeColorRandomly(void) {
    u16 red = getRandomNumber() % 1000;  // 确保值在0到999之间
    u16 green = getRandomNumber() % 1000;
    u16 blue = getRandomNumber() % 1000;
    
    TIM5->CCR2 = red;
    TIM5->CCR3 = green;
    TIM5->CCR4 = blue;
}



//超声波初始化
void SR04_Init(void){
	
	//用于ECHO捕获的定时器4的初始化
	tim4_SRECHO_CaptureInit();
	
	//用于发射高电平的PB8的初始化
	GPIOB->MODER &= ~(3U<<16);
	GPIOB->MODER |= (1U<<16);
	
	GPIOB->OTYPER |= (1U<<8);
	
	GPIOB->OSPEEDR &= ~(3U<<16);
	GPIOB->OSPEEDR |= (1U<<17);
	
	GPIOB->PUPDR &= ~(3U<<16);
	//先默认低电平
	GPIOB->ODR &= ~(1U<<8);
	//printf("超声波初始化完成");
}

//超声波开始工作
//通过PB8持续输出高电平10us,用以超声波的trig
void SR04_Start(void){
	GPIOB->ODR |= (1U<<8);
	
	baseTim6Delay_Us(10);
	
	GPIOB->ODR &= ~(1U<<8);
	//printf("启动一次超声波");
}


//光敏传感器的初始化
void LightSensor(void){
	//初始化ADC1的相关配置
	adc1_PC2_ch12_init();
}

//毒气检测的初始化
void gasDetection(void){
	//初始化ADC2的相关配置
	adc2_PC0_ch10_init();
}

//光敏和毒气同ADC初始化
void light_gas_detection_init(void){
	//初始化ADC1的相关配置
	adc1_PC2PC0_ch12ch10ch16_init();
}

//三通道使用中断方式获取数据
void light_gas_temp_interrupt_init(void){
	adc1_interrupt_init();
}