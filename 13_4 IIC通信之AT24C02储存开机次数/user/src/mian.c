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
	//NVIC_SetPriorityGrouping(5); // 抢占优先级 2 位，响应优先级 2 位
	
	
	//所有初始化
	
	//蜂鸣器初始化
	beepInit();
	
	//串口1初始化以便打印信息,以及发送信息
	usart1Init(115200);
	
	//AT24C02的初始化
	at24c02_init();
	
	
	
	//先开后关
	BEEP_ON;
	baseTim6Delay_Ms(50);
	BEEP_OFF;
	
	PrintBootCount();
	
	while(1){
		
	}
}