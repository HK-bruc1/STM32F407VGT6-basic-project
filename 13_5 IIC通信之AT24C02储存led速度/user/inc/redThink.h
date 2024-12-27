#ifndef REDTHINK_H
#define REDTHINK_H
#include "stm32f4xx.h"                  // Device header

//现在有是1，没有是0
#define IR_NO 0
#define IR_OK 1

#define IRKEY_UNPRESS 0
#define IRKEY_PRESS 1


void redInit();
u8 irData();
u8 irKey_scan(void);
#endif