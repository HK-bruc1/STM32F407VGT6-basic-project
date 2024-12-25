#ifndef NVIC_H
#define NVIC_H
#include "stm32f4xx.h"                  // Device header
#include "adc.h" 

// 声明全局变量与结构体
typedef	struct{
	u8 usart1_buff[50];
	u32 length;
	u8 usart1StrRec_flag;
}USART1BAL_t;

//对外声明结构体变量
extern USART1BAL_t usart1_val;

extern volatile u8 countFlag;
extern volatile u32 count;

extern volatile u32 sysTickCount[5];
extern volatile u32 TIM7Count[10];
extern volatile u32 TIM6Count[5];
extern volatile u8 IR_statu;

#endif