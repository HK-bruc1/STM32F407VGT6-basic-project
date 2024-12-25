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

//此源文件用于定义综合性函数，不定义各外设的初始化配置，调用时都是配置好的


volatile uint8_t randomColorFlag = 0;


// 封装AT24C02写入结构体操作的函数，传入结构体指针即可，避免副本浪费空间
void writeBookToEEPROM(u8 startAddress, const Book* book) {
	//地址为0~255，u8足够
	u8 address = startAddress;
	
	// 写入name,计算结构体成员的字节大小并逐个写入
	//跨页写入多字节,内部起始地址，需要写入数据的地址，写入字节的数量
	//book->name 等同于 (&book->name[0])，即 name 数组的首元素的地址
	//u8 name[30]与char *类型不匹配，需要强转？
	at24c02_skip_page_write(address,(char *)book->name,sizeof(book->name));
	
	//下一次写入的内部地址为
	address+=sizeof(book->name);
	
	//写入作者名
	at24c02_skip_page_write(address,(char *)book->writer,sizeof(book->writer));
	
	//下一次写入的内部地址为
	address+=sizeof(book->writer);
	
	//写入编号
	at24c02_skip_page_write(address,(char *)book->number,sizeof(book->number));
	
	//下一次写入的内部地址为
	address+=sizeof(book->number);
	
	//写入热度,以下直接写入单个字节就可以，价格单独输入？以字符串写入？？？





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