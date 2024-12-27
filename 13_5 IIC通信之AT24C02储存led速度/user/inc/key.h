#ifndef KEY_H
#define KEY_H
#include "stm32f4xx.h"                  // Device header

//KEY1不按是0，按了是1，第0位直接判断是0还是1
//KEY2，3，4 没按下是高电平，按了会被拉低
//移动过去在移动回来成为最低位，使用！直接判断是0还是1
//按了是1，没按是0
#define KEY1 ((GPIOA->IDR) & 1U)
#define KEY2 (!(((GPIOE->IDR) & (1U<<2))>>2))
#define KEY3 (!(((GPIOE->IDR) & (1U<<3))>>3))
#define KEY4 (!(((GPIOE->IDR) & (1U<<4))>>4))

// 按键状态定义
#define KEY1_PRESS   1
#define KEY2_PRESS   2
#define KEY3_PRESS   3
#define KEY4_PRESS   4
#define KEY_UNPRESS  0

void keyInit();
u8 key_scan(void);
#endif