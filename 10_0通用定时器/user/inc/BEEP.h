#ifndef BEEP_H
#define BEEP_H

//单独开一个宏定义用于开蜂鸣器开关
#define BEEP_ON (GPIOE->ODR |= 1)

#define BEEP_OFF (GPIOE->ODR &= 0)

//翻转蜂鸣器
#define BEEP_TURN GPIOE->ODR^=(1<<0)


void beepInit();

#endif