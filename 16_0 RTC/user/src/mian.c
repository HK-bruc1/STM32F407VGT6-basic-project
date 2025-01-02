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
#include "w25q64.h"
#include "iwdg.h"
#include "rtc.h"


int main(){
	//设置优先级分组（只有一个，放在主函数）
	NVIC_SetPriorityGrouping(5); // 抢占优先级 2 位，响应优先级 2 位
	
	
	//所有初始化
	
	//蜂鸣器初始化
	beepInit();
	
	//串口1初始化以便打印信息，以及发送信息
	usart1Init(115200);
	//RTC初始化
	rtc_init();
	
	//定时器7初始化
	baseTim7Interrupt_Init(1);
	
	
	
	//先开后关
	BEEP_ON;
	baseTim6Delay_Ms(50);
	BEEP_OFF;
	
	
	
	while(1){
		//每隔一秒打印一次万年历
		baseTim7DelayPlus_Ms(1000);
		RTC_T current_time =  rtc_get();
		// 格式化输出，增加可读性
        printf("Current Time: %04d-%02d-%02d Week:%d %02d:%02d:%02d\n", 
               current_time.years, 
               current_time.mon, 
               current_time.date, 
               current_time.week,
               current_time.h, 
               current_time.m, 
               current_time.s);
	}
}