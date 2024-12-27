#ifndef SYSTICK_H
#define SYSTICK_H
#include "stm32f4xx.h"                  // Device header


//系统滴答定时器中断初始化
void sysTickInterrupt_init(u16 ms);

#endif