#ifndef WANG_TIME_H
#define WANG_TIME_H
#include "stm32f4xx.h"                  // Device header

void delay_ms(u32 ms);
void sysTickDelay_ms(u16 ms);
void sysTickDelayPlus_Ms(u32 ms);
void baseTim6Delay_Ms(u32 ms);
void baseTim6DelayPlus_Ms(u32 ms);
void generalTim7DelayPlus_Ms(u32 ms);

#endif