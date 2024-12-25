#include "systick.h"
#include "stm32f4xx.h"                  // Device header
#include "selfprintf.h"                  // Device header


//系统滴答定时器中断初始化函数
void sysTickInterrupt_init(u16 ms){
	//时钟源选择
	SysTick->CTRL &= ~(1U<<2);
	
	//写入重载值
	SysTick->LOAD = 21000 * ms - 1;
	
	//清空当前值寄存器
	SysTick->VAL = 0;
	
	//开启系统滴答定时器的中断使能
	SysTick->CTRL |= (1U<<1);
	
	/*NVIC控制器配置*/
		//优先级分组(在主函数所有初始化的前面)
		//计算编码值
		u32 pri = NVIC_EncodePriority(5, 1, 2);
		//设置具体中断源
		NVIC_SetPriority(SysTick_IRQn, pri);
		//启用对应通道使能中断,没有对应通道，不用使能
		//NVIC_EnableIRQ(SysTick_IRQn);
	
	//使能定时器
	SysTick->CTRL |= (1U<<0);
}