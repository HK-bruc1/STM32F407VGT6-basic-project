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


int main(){
	//设置优先级分组为分组（只有一个，放在主函数）
	NVIC_SetPriorityGrouping(5); // 抢占优先级 2 位，响应优先级 2 位
	
	//所有初始化
	
	//蜂鸣器初始化关闭
	beepInit();
	//led初始化
	ledInit();
	//串口1初始化以便打印信息
	usart1Init(115200);
	
	//IIM6定时中断初始化,1ms进一次中断
	baseTim6Interrupt_Init(1);
	//baseTim7Interrupt_Init(1);
	//通用定时器3的初始化
	//tim3Init();
	
	
	//先开后关
	BEEP_ON;
	baseTim6DelayPlus_Ms(50);
	BEEP_OFF;
	
	
	while(1){
		
	}
}