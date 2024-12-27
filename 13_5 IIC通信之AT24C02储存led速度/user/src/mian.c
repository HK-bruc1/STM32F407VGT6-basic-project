#include "stm32f4xx.h"                  // Device header
#include "LED.h"
#include "wang_time.h"
#include "BEEP.h"
#include "key.h"
#include "usart.h"
#include "nvic.h"
#include "selfprintf.h"
#include "exti.h"
#include "systick.h"
#include "baseTIM.h"
#include "GeneralTim.h"
#include "redThink.h"
#include "functions.h"
#include "RNG.h"
#include "event_handler.h"
#include "at24c02.h"
#include "string.h"


int main(){
	//设置优先级分组为分组（只有一个，放在主函数）
	NVIC_SetPriorityGrouping(5); // 抢占优先级 2 位，响应优先级 2 位
	
	
	//所有初始化
	
	//蜂鸣器初始化
	beepInit();
	//按键初始化
	keyInit();
	//初始化led
	ledInit();
	
	//串口1初始化以便打印信息,以及发送信息
	usart1Init(115200);
	
	//AT24C02的初始化
	at24c02_init();
	
	//定时器7的开启,每1ms进入一次中断
	baseTim7Interrupt_Init(1);
	
	
	//先开后关
	//BEEP_ON;
	baseTim6Delay_Ms(50);
	//BEEP_OFF;
	
	
	//按键值
	u8 key_val = 0;
	//0地址用于单独存储开机次数
	u8 baseAddr = 1;
	
	//初始化一个流水灯结构体实例
	WaterLed_t waterLed;
	//保存此次开机次数并返回开机次数
	waterLed.open_count =  PrintBootCount();
	//如果是首次开机的话
	if(waterLed.open_count==1){
		printf("FirstOpen!\r\n");
		//设置速度初始值
		Tim7WaterLed_Speed = 5;
		//赋值首次开机标志
		waterLed.open_Flag = 0x01;
	}else {
		//如果不是首次开机速度读取存的数据，其他数据不需要
		//使用连续读取函数直接读取整个结构体
    at24c02_page_read(baseAddr, (char*)&waterLed, sizeof(waterLed));
		//使用读取的数据拿出来赋值速度即可
		Tim7WaterLed_Speed = waterLed.speed;
		printf("非首次开机！\r\n");
		//0x01 = 0000 0001
		printf("首次开机标志:%d\r\n",waterLed.open_Flag);
		printf("LastSpeed:%d\r\n",waterLed.speed);
	}
	
	
	while(1){
		key_val = key_scan();
		switch(key_val){
			case 1: 
				//开启流水灯
				waterLedOpen_Flag = 1;
				//按下k1后三秒无操作，保存数据到eeprom，把保存标志置1
				sava_Flag = 1;
				//非阻塞式，在等待时，也扫描按键状态，准备随时改动sava_Flag
				baseTim7DelayPlus_Ms(3000);
				break;
			case 2:
				if(Tim7WaterLed_Speed < 10) {
        Tim7WaterLed_Speed++;
				}
				//其他按键都可以打断保存状态
				sava_Flag = 0;
				break;
			case 3: 
				//其他按键都可以打断保存状态
				sava_Flag = 0;
				break;
			case 4:
				if(Tim7WaterLed_Speed>0){
					Tim7WaterLed_Speed--;
				}
				//其他按键都可以打断保存状态
				sava_Flag = 0;
				break;
			default: 
				break;
		}
		//如果3秒期间没人修改就保存
		if(sava_Flag){
			//将数据写入结构体，此时结构体三个成员已经赋值
			waterLed.speed = Tim7WaterLed_Speed;
			//直接调用连续写，拿结构体指针
			//使用跨页写入函数直接写入整个结构体
			at24c02_skip_page_write(baseAddr, (char*)&waterLed, sizeof(waterLed));
			printf("save success!%d %d %d",waterLed.open_count,waterLed.open_Flag,waterLed.speed);
			//复位保存状态
			sava_Flag = 0;
			//写完之后滴答一声
			//BEEP_ON;
			baseTim6Delay_Ms(50);
			//BEEP_OFF;
		}
		
	}
}