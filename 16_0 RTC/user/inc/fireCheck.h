#ifndef FIRECHECK_H
#define FIRECHECK_H
#include "stm32f4xx.h"                  // Device header

//有火是低电平，没有是高电平
//现在有是1，没有是0
#define FIRE !((GPIOC->IDR & (1<<9))>>9)
void fireInit();

#endif