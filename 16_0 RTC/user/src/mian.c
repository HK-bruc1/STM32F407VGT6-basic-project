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
	
	//手动校准时间
//	RTC_T data = {
//    .years = 2025,
//    .mon = 1,
//    .date = 3,
//    .week = 5,
//    .h = 12,
//    .m = 12,
//    .s = 12
//	};
//	rtc_set(data);
	
	
	while(1){
		//每隔一秒打印一次万年历
		baseTim7DelayPlus_Ms(1000);
		RTC_T current_time =  rtc_get();
		// 格式化输出当前时间，增加可读性
		printf("当前时间：%04d年%02d月%02d日 星期%d %02d:%02d:%02d\n",
			current_time.years,  // 年份
			current_time.mon,     // 月份
			current_time.date,    // 日期
			current_time.week,    // 星期几（1-7，7表示星期日）
			current_time.h,       // 小时
			current_time.m,       // 分钟
			current_time.s);      // 秒
	}
}