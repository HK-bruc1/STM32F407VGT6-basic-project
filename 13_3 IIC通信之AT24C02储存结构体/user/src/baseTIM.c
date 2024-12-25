#include "baseTIM.H"
#include "stm32f4xx.h"                  // Device header
#include "LED.h"

//基本定时器6中断初始化
void baseTim6Interrupt_Init(u32 ms){
	//片上外设使能,为了配置生效
	RCC->APB1ENR |= (1U<<4); 
	
	
	//CR1
	//使用影子寄存器，第七位置1
	TIM6->CR1 |= (1U<<7);
	//选择循环计数模式,第三位置0
	TIM6->CR1 &= ~(1U<<3);
	//触发中断的条件:计数器上溢/下溢 || 将 UG 位置 1
	TIM6->CR1 &= ~(1U<<2);
	//生成更新事件的条件第一位置0：计数器上溢/下溢 || 将 UG 位置 1
	TIM6->CR1 &=~(1U<<1);
	
	
	//预分频器配置,8400分频 速度为10次/ms
	//延时30ms那么总计数的值为30*10 = 300；
	TIM6->PSC = 8400-1;
	
	
	//自动重载寄存器
	TIM6->ARR = ms*10-1;
	
	
	//人为生成一个更新事件,把值装载进计数器
	TIM6->EGR |= (1U<<0);
	
	//清除标志位（人为更新了一次事件）
	TIM6->SR &= ~(1U<<0);
	
	//开启中断寄存器的中断功能
	TIM6->DIER |= (1U<<0);
	
	//所有配置搞定后，最后使能计数器
	TIM6->CR1 |= (1U<<0);
	
	
	//片上外设还需要NVIC来管理和使能中断
	// 1. 设置优先级分组为分组（只有一个，放在主函数）
	//NVIC_SetPriorityGrouping(5); // 抢占优先级 2 位，响应优先级 2 位

	// 2. 计算优先级编码值
	u32 pri = NVIC_EncodePriority(5, 1, 3); // 分组值5，抢占优先级 1，响应优先级 2

	// 3. 设置到具体的中断源 TIM6_IRQn 的优先级
	//共享的中断通道，记得在中断服务函数中判断一下是什么中断信号
	NVIC_SetPriority(TIM6_DAC_IRQn, pri);

	// 4. 启用 TIM6_IRQn 中断
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
	
}


//基本定时器7中断初始化
void baseTim7Interrupt_Init(u32 ms){
	//片上外设使能,为了配置生效
	RCC->APB1ENR |= (1U<<5); 
	
	
	//CR1
	//使用影子寄存器，第七位置1
	TIM7->CR1 |= (1U<<7);
	//选择循环计数模式,第三位置0
	TIM7->CR1 &= ~(1U<<3);
	//触发中断的条件:计数器上溢/下溢 || 将 UG 位置 1
	TIM7->CR1 &= ~(1U<<2);
	//生成更新事件的条件第一位置0：计数器上溢/下溢 || 将 UG 位置 1
	TIM7->CR1 &=~(1U<<1);
	
	
	//预分频器配置,8400分频 速度为10次/ms
	//延时30ms那么总计数的值为30*10 = 300；
	TIM7->PSC = 8400-1;
	
	
	//自动重载寄存器,1ms计数10次，ARR写入9即可
	TIM7->ARR = ms*10-1;
	
	
	//人为生成一个更新事件,把值装载进计数器
	TIM7->EGR |= (1U<<0);
	
	//清除标志位（人为更新了一次事件）
	TIM7->SR &= ~(1U<<0);
	
	//开启中断寄存器的中断功能
	TIM7->DIER |= (1U<<0);
	
	//所有配置搞定后，最后使能计数器
	TIM7->CR1 |= (1U<<0);
	
	
	//片上外设还需要NVIC来管理和使能中断
	// 1. 设置优先级分组为分组（只有一个，放在主函数）
	//NVIC_SetPriorityGrouping(5); // 抢占优先级 2 位，响应优先级 2 位

	// 2. 计算优先级编码值
	u32 pri = NVIC_EncodePriority(5, 1, 1); // 分组值5，抢占优先级 1，响应优先级 2

	// 3. 设置到具体的中断源 TIM7_IRQn 的优先级
	NVIC_SetPriority(TIM7_IRQn, pri);

	// 4. 启用 TIM7_IRQn 中断
	NVIC_EnableIRQ(TIM7_IRQn);
	

}