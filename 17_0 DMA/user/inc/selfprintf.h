#ifndef SELFPRINTF_H
#define SELFPRINTF_H
#include "stm32f4xx.h"                  // Device header
#include <stdio.h>

// 声明可能需要的函数
int fputc(int ch, FILE *f);
void _sys_exit(int x);

#endif