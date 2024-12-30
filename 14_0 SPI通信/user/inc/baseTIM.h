#ifndef BASETIM_H
#define BASETIM_H
#include "stm32f4xx.h"                  // Device header

void baseTim6Interrupt_Init(u32 ms);
void baseTim7Interrupt_Init(u32 ms);

#endif