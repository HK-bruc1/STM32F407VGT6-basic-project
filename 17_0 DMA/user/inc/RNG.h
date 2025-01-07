#ifndef RNG_H
#define RNG_H
#include "stm32f4xx.h"                  // Device header


void RNG_Init(void);
u32 getRandomNumber(void);



#endif