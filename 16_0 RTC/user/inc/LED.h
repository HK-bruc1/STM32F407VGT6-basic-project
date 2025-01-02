#ifndef __LED_H
#define __LED_H
#include "stm32f4xx.h"                  // Device header

 
//4个灯的开
#define LED1_ON GPIOC->ODR &= ~(1 << 4)
#define LED2_ON GPIOC->ODR &= ~(1 << 5)
#define LED3_ON GPIOC->ODR &= ~(1 << 6)
#define LED4_ON GPIOC->ODR &= ~(1 << 7)

//除开初始化的，还要单独控制
#define LED1_OFF GPIOC->ODR |= (1 << 4)
#define LED2_OFF GPIOC->ODR |= (1 << 5)
#define LED3_OFF GPIOC->ODR |= (1 << 6)
#define LED4_OFF GPIOC->ODR |= (1 << 7)

//单独翻转
#define LED1_TURN GPIOC->ODR ^= (1 << 4)
#define LED2_TURN GPIOC->ODR ^= (1 << 5)
#define LED3_TURN GPIOC->ODR ^= (1 << 6)
#define LED4_TURN GPIOC->ODR ^= (1 << 7)


int ledInit(void);
void openAllLed();
void closeAllLed();
void ledX_on(u8 ledNum);
void ledX_off(u8 ledNum);
void waterLed(u8 speed);

#endif