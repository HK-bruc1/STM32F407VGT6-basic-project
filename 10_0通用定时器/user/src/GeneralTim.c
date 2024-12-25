#include "GeneralTim.h"
#include "stm32f4xx.h"                  // Device header
#include "key.h"
#include "wang_time.h"


void tim3Init(void){
	/*IO控制器配置*/
	//端口时钟使能 PC6
	RCC->AHB1ENR |= (1U<<2);
	//端口模式配置-------复用模式
	GPIOC->MODER &= ~(3U<<12);
	GPIOC->MODER |= (1U<<13);
	//输出类型
	GPIOC->OTYPER &= ~(1U<<6);
	//输出速度
	GPIOC->OSPEEDR &= ~(3U<<12);
	GPIOC->OSPEEDR |= (1U<<13);
	//上下拉
	GPIOC->PUPDR &= ~(3U<<12);
	//复用功能寄存器
	GPIOC->AFR[0] &= ~(15U<<24);
	GPIOC->AFR[0] |= (1U<<25);
	
	/*通用定时器控制器配置*/
	//定时器时钟使能
	RCC->APB1ENR |= (1U<<1);
	//CR1
	//各影子寄存器
	TIM3->CR1 |= (1U<<7);
	//边沿对齐
	TIM3->CR1 &= ~(3U<<5);
	TIM3->CR1 &= ~(1U<<4);
	TIM3->CR1 &= ~(1U<<3);
	TIM3->CR1 &= ~(1U<<1);
	
	//SMCR
	//时钟源来自内部时钟
	TIM3->SMCR &= ~(7U<<0);
	
	//CCMRx 
	TIM3->CCMR1 &= ~(3U<<0);
	TIM3->CCMR1 |= (1U<<3);
	TIM3->CCMR1 &= ~(7U<<4);
	TIM3->CCMR1 |= (3U<<5);
	
	//CCER
	TIM3->CCER &= ~(1U<<3);
	//LED低电平为有效，所以设置低电平
	TIM3->CCER |= (1U<<1);
	//PSC 84000000MHZ 84000/ms---->1000MHZ 84分频 1ms 1000个数
	TIM3->PSC = 83;
	//ARR
	TIM3->ARR = 1000;
	//CCRx
	TIM3->CCR1 = 1000;
	//EGR 手动生成更新事件，装载值
	TIM3->EGR |= (1U<<0); 
	
	//tim3通道1输出使能
	TIM3->CCER |= (1U<<0);
	//计数器使能
	TIM3->CR1 |= (1U<<0);
}


//通过PWM的占空比来控制LED3的亮度
void SetLED_3Brightness(void){
	  u8 key_val = 0;
		key_val =  key_scan();
		if(key_val == KEY2_PRESS){
			if(TIM3->CCR1<1000){
				TIM3->CCR1+=100;
			}
		}
		if(key_val == KEY4_PRESS){
			if(TIM3->CCR1>=100){
				TIM3->CCR1-=100;
			}
		}
}

//通用定时器3 LED3呼吸灯效果 循环控制占空比的值
void LED3_BreathingEffect(void){
	u16 i;
	static short step = 10;
	for(i=0;i<100;i++){
		TIM3->CCR1 -= step;
		//baseTim6Delay_Ms(10);
		sysTickDelayPlus_Ms(10);
	}
	step = -step;
}
