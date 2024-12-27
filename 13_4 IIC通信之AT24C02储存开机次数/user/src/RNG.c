#include "RNG.h"
#include "stm32f4xx.h"                  // Device header

//初始化
void RNG_Init(void) {
    // 启用RNG时钟
    RCC->AHB2ENR |= (1<<6);  // ENG 时钟开启
    
    // 启用RNG
    RNG->CR |= (1<<2);       // 随机数使能开启
}

//生成随机数,先初始化再使用
u32 getRandomNumber(void) {
    u32 randnum;
		while(!(RNG->SR&(1<<0)));	//等待随机数就绪  
		randnum = RNG->DR;//别在这里取模，保持随机数的稳定性
		return randnum ;
}

