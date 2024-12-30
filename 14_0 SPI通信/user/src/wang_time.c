#include "stm32f4xx.h"                  // Device header
#include "wang_time.h"
#include "systick.h"
#include "nvic.h"
#include "GeneralTim.h"
#include "functions.h"
#include "key.h"
#include "at24c02.h"

//不建议使用，阻塞式延迟
//u32的类型别名定义在stm32f4xx.h头文件中
void delay_ms(u32 ms)
{
	u32 i = 168 / 4 * 1000 * ms;
	while(i)
	{
		i--;
	}
}

//不建议使用（后期会与freeRTOS调度时间片冲突）
//利用系统滴答定时器重构（阻塞式延迟）
//u16:short 65535
//ms最多798
void sysTickDelay_ms(u16 ms){
	//时钟源选择
	SysTick->CTRL &= ~(1U<<2);
	
	//写入重载值
	SysTick->LOAD = 21000 * ms - 1;
	
	//清空当前值寄存器
	SysTick->VAL = 0;
	
	//使能定时器
	SysTick->CTRL |= (1U<<0);
	
	//等待计数器计数完成   控制及状态寄存器的16号位	
	while(!(SysTick->CTRL & (1U<<16)));
	
	//关闭定时器           控制及状态寄存器的0号位
	//延时用的话，就单次计数
	SysTick->CTRL &= ~(1U<<0);
}


//不建议使用（后期会与freeRTOS调度时间片冲突）
//非阻塞式延迟函数重构
//但是初始化时，中断频率要设置为1ms
void sysTickDelayPlus_Ms(u32 ms) {
	sysTickCount[1] = ms;
	while(sysTickCount[1]) {
			// 非阻塞等待
	}
}


//利用基本定时器重构延迟函数,利用基本定时器的定时器
//阻塞式
void baseTim6Delay_Ms(u32 ms){
    //片上外设使能
    RCC->APB1ENR |= (1U<<4); 
    
    // CR1配置
    // 使用影子寄存器，第七位置1
    TIM6->CR1 |= (1U<<7);
    // 选择单次计数模式,第三位置1
    TIM6->CR1 |= (1U<<3);
    // 生成更新事件的条件第一位置0：计数器上溢/下溢 || 将 UG 位置 1
    TIM6->CR1 &=~(1U<<1);
    
    // 预分频器配置,8400分频，速度为10次/ms
    TIM6->PSC = 8399; // 84 MHz / 8400 = 10 kHz, 每毫秒计数10次
    
    //自动重载寄存器
    TIM6->ARR = ms * 10 - 1; // 每毫秒计数10次，所以乘以10
    
    //人为生成一个更新事件,把值装载进计数器
    TIM6->EGR |= (1U<<0);
    
    // 清除标志位（人为更新了一次事件）
    TIM6->SR &= ~(1U<<0);
    
    //最后使能计数器
    TIM6->CR1 |= (1U<<0);
    
    //等待更新事件出现（计数完成）
    while(!(TIM6->SR & (1U<<0)));
}


void baseTim6Delay_Us(u32 us){
    //片上外设使能
    RCC->APB1ENR |= (1U<<4); 
    
    // CR1配置
    // 使用影子寄存器，第七位置1
    TIM6->CR1 |= (1U<<7);
    // 选择单次计数模式,第三位置1
    TIM6->CR1 |= (1U<<3);
    // 生成更新事件的条件第一位置0：计数器上溢/下溢 || 将 UG 位置 1
    TIM6->CR1 &=~(1U<<1);
    
    // 预分频器配置,42分频，速度为2次/us
    TIM6->PSC = 41; // 84 MHz / 42 = 2 MHz, 每微秒计数2次
    
    //自动重载寄存器
    TIM6->ARR = us * 2 - 1; // 每微秒计数2次，所以乘以2
    
    //人为生成一个更新事件,把值装载进计数器
    TIM6->EGR |=(1U<<0);
    
    //清除标志位（人为更新了一次事件）
    TIM6->SR &= ~(1U<<0);
    
    //最后使能计数器
    TIM6->CR1 |= (1U<<0);
    
    //等待更新事件出现（计数完成）
    while(!(TIM6->SR & (1U<<0)));
}


//利用通用定时器重构延迟函数,利用通用定时器的定时器
//阻塞式
void baseTim11Delay_Ms(u32 ms){
	//片上外设使能
	RCC->APB2ENR |= (1U<<18); 
	
	//CR1
	//第八位和第九位 CKD：时钟分频
	
	//使用影子寄存器，第七位置1
	TIM11->CR1 |= (1U<<7);
	//选择单次计数模式,第三位置1
	TIM11->CR1 |= (1U<<3);
	//生成更新事件的条件第一位置0：计数器上溢/下溢 || 将 UG 位置 1
	TIM11->CR1 &=~(1U<<1);
	
	//预分频器配置,8400分频 速度为10次/ms
	//延时30ms那么总计数的值为30*10 = 300；
	TIM11->PSC = 8399;
	
	//自动重载寄存器
	TIM11->ARR = ms*10-1;
	
	//人为生成一个更新事件,把值装载进计数器
	TIM11->EGR |=(1U<<0);
	
	//清除标志位（人为更新了一次事件）
	TIM11->SR &= ~(1U<<0);
	
	//最后使能计数器
	TIM11->CR1 |= (1U<<0);
	
	//等待更新事件出现（计数完成）
	//没有发生更新事件一直等待
	while(!(TIM6->SR & (1U<<0)));
}

//非阻塞式延迟函数重构
//但是初始化时，中断频率要设置为1ms
void baseTim6DelayPlus_Ms(u32 ms) {
	TIM6Count[1] = ms;
	while(TIM6Count[1]) {
		// 等待时可以做的事情：非阻塞等待
		//等待时也不能阻塞按键扫描，这里再调用一次，虽然不轻量
		//不延迟，就不用调用了，不冲突。
	}
}

//定时器7的非阻塞式延迟
void baseTim7DelayPlus_Ms(u32 ms) {
	TIM7Count[9] = ms;
	while(TIM7Count[9]) {
		// 等待时可以做的事情：非阻塞等待
		//等待时也不能阻塞按键扫描，这里再调用一次，虽然不轻量
		//不延迟，就不用调用了，不冲突。
		if(key_scan()==2 || key_scan()==3 || key_scan()==4){
			//如果按下就会非零进来一次，我立马修改sava_flag,暂停保存
			sava_Flag = 0;
		}
	}
}

