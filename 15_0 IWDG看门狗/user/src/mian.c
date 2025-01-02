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


int main(){
	//设置优先级分组为分组（只有一个，放在主函数）
	//NVIC_SetPriorityGrouping(5); // 抢占优先级 2 位，响应优先级 2 位
	
	
	//所有初始化
	
	//蜂鸣器初始化
	beepInit();
	//led初始化
	ledInit();
	
	//串口1初始化以便打印信息,以及发送信息
	usart1Init(115200);
	
	//看门狗初始化
	iwdg_init();
	
	//定时器7初始化
	baseTim7Interrupt_Init(1);
	
	
	
	//先开后关
	//BEEP_ON;
	baseTim6Delay_Ms(50);
	//BEEP_OFF;
	printf("IWDG_reset!\r\n");
	
	
	
	while(1){
		//3000时间，1000喂狗
		baseTim7DelayPlus_Ms(1000);
		iwdg_feed();
		printf("feed_Dog!\r\n");
	}
}