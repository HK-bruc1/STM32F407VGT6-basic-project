#include "stm32f4xx.h"                  // Device header
#include "BEEP.h"
#include "wang_time.h"

void beepInit(){

// 使能GPIOE时钟,对应总线打开
RCC->AHB1ENR |= (1U << 4);
	
	
// 配置PE0为通用输出模式
GPIOE->MODER &= ~3;  // 清除PC4原模式配置 
GPIOE->MODER |= 1;   // 设置PC4为通用输出模式
	
// 设置推挽输出
GPIOE->OTYPER &= ~1;
	
// 配置速度（中速）
GPIOE->OSPEEDR &= ~3;  // 清除速度配置 
GPIOE->OSPEEDR |= (1U << 0);   // 设置为中速模式
GPIOE->OSPEEDR |= (1U << 1);   // 设置为中速模式
	
// 配置上下拉（无上下拉）
GPIOE->PUPDR &= ~3;
	
//初始化是蜂鸣器的关闭
GPIOE->ODR &= 0;
	

}