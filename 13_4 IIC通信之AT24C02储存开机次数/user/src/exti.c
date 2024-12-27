#include "exti.h"
#include "stm32f4xx.h"                  // Device header
#include "wang_time.h"
#include "LED.h"
#include "selfprintf.h"


//使PC8连接到EXTI8,用作外部中断依据
void exti8Init(){
	/*IO控制器配置*/
		//端口时钟使能 PC8
		RCC->AHB1ENR |= (1U<<2);
		//端口模式配置 检测GPIO口的高低电平 通用输入
		GPIOC->MODER &= ~(3U<<16);
		//上下拉
		GPIOC->PUPDR &= ~(3U<<16);
	
	
	/*EXTI控制器配置*/
		//系统配置控制器时钟使能------使用寄存器之前要先使能（不然配置不生效），EXTI控制器没有所以不用配置，他们两个共用的
		RCC->APB2ENR |= (1<<14);
		//将需要用的IO口映射到对应的EXTI8接口上
		SYSCFG->EXTICR[2] &= ~(15U<<0);
		SYSCFG->EXTICR[2] |= (1U<<1);
		//上升沿/下降沿触发 按键按下，是高电平，上升沿,松开也进
		EXTI->RTSR |= (1<<8);
		EXTI->FTSR |= (1<<8);
		//中断屏蔽寄存器------相当于中断使能
		EXTI->IMR |= (1<<8);

	/*NVIC控制器配置*/
		// 1. 设置优先级分组为分组（只有一个，放在主函数）
    //NVIC_SetPriorityGrouping(5); // 抢占优先级 2 位，响应优先级 2 位

    // 2. 计算优先级编码值
    u32 pri = NVIC_EncodePriority(5, 1, 2); // 分组值5，抢占优先级 1，响应优先级 2

    // 3. 设置到具体的中断源 EXTI9_5_IRQHandler 的优先级
    NVIC_SetPriority(EXTI9_5_IRQn, pri);

    // 4. 启用 EXTI0_IRQn 中断
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}
