#ifndef NVIC_H
#define NVIC_H
#include "stm32f4xx.h"                  // Device header

// ����ȫ�ֱ�����ṹ��
typedef	struct{
	u8 usart1_buff[50];
	u32 length;
	u8 usart1StrRec_flag;
}USART1BAL_t;

//���������ṹ�����
extern USART1BAL_t usart1_val;

extern volatile u8 countFlag;
extern volatile u32 count;

extern volatile u32 sysTickCount[5];
extern volatile u32 TIM7Count[5];
extern volatile u32 TIM6Count[5];

#endif