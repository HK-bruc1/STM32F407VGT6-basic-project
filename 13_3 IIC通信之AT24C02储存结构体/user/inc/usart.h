#ifndef USART_H
#define USART_H
#include "stm32f4xx.h"                  // Device header


//函数声明和宏定义
void usart1Init(u32 baud);
void USART1_NVIC_init(void);
void usart1_send_byte(u8 data);
u8 usart1WaterLed(void);
u8 usart1Servo(void);

#endif