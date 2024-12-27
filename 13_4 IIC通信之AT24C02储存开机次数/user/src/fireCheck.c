#include "fireCheck.h"
#include "stm32f4xx.h"                  // Device header

//初始化PC9端口的状态
void fireInit(){
	//总线对应的时钟使能打开
	RCC->AHB1ENR |= (1<<2);

	//清空第18和19位，置0,其他位不动
	GPIOC->MODER &= ~(3U<<18);

	//清空第16和17位，置0,其他位不动
	GPIOC->PUPDR &= ~(3U<<18);
}
