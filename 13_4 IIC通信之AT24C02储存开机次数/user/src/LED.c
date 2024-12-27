#include "stm32f4xx.h"                  // Device header
#include "LED.h"
#include "wang_time.h"
#include "BEEP.h"

//初始化各端口状态
int ledInit(void){
// 使能GPIOC时钟,总线打开
RCC->AHB1ENR |= (1<<2);

/*
	
// 配置PC4为通用输出模式
GPIOC->MODER &= ~(3 << 8);  // 清除PC4原模式配置 
GPIOC->MODER |= (1 << 8);   // 设置PC4为通用输出模式
// 设置推挽输出
GPIOC->OTYPER &= ~(1 << 4);
// 配置速度（中速）
GPIOC->OSPEEDR &= ~(3 << 8);  // 清除速度配置 
GPIOC->OSPEEDR |= (1 << 8);   // 设置为高速模式
GPIOC->OSPEEDR |= (1 << 9); 
	
// 配置上下拉（无上下拉）
GPIOC->PUPDR &= ~(3 << 8);
//led1的初始化
GPIOC->ODR |= (1 << 4);


// 配置PC5为通用输出模式
GPIOC->MODER &= ~(3 << 10);  // 清除PC5原模式配置 
GPIOC->MODER |= (1 << 10);   // 设置PC5为通用输出模式
// 设置推挽输出
GPIOC->OTYPER &= ~(1 << 5);
// 配置速度（中速）
GPIOC->OSPEEDR &= ~(3 << 10);  // 清除速度配置 
GPIOC->OSPEEDR |= (1 << 10);   
GPIOC->OSPEEDR |= (1 << 11); 

// 配置上下拉（无上下拉）
GPIOC->PUPDR &= ~(3 << 10);
// LED2的初始化（关闭LED）
GPIOC->ODR |= (1 << 5);

// 配置PC6为通用输出模式
GPIOC->MODER &= ~(3 << 12);  // 清除PC6原模式配置 
GPIOC->MODER |= (1 << 12);   // 设置PC6为通用输出模式
// 设置推挽输出
GPIOC->OTYPER &= ~(1 << 6);
// 配置速度（中速）
GPIOC->OSPEEDR &= ~(3 << 12);  // 清除速度配置 
GPIOC->OSPEEDR |= (1 << 12);   
GPIOC->OSPEEDR |= (1 << 12);   

// 配置上下拉（无上下拉）
GPIOC->PUPDR &= ~(3 << 12);
// LED3的初始化（关闭LED）
GPIOC->ODR |= (1 << 6);

// 配置PC7为通用输出模式
GPIOC->MODER &= ~(3 << 14);  // 清除PC7原模式配置 
GPIOC->MODER |= (1 << 14);   // 设置PC7为通用输出模式
// 设置推挽输出
GPIOC->OTYPER &= ~(1 << 7);
// 配置速度（中速）
GPIOC->OSPEEDR &= ~(3 << 14);  // 清除速度配置 
GPIOC->OSPEEDR |= (1 << 14);   
GPIOC->OSPEEDR |= (1 << 15);   

// 配置上下拉（无上下拉）
GPIOC->PUPDR &= ~(3 << 14);
// LED4的初始化（关闭LED）
GPIOC->ODR |= (1 << 7);

*/

	// 配置PC4-PC7为通用输出模式
	GPIOC->MODER &= ~((3U << 8) | (3U << 10) | (3U << 12) | (3U << 14));  // 清除原模式
	GPIOC->MODER |= ((1U << 8) | (1U << 10) | (1U << 12) | (1U << 14));   // 设置为通用输出

	// 设置为推挽输出
	GPIOC->OTYPER &= ~((1U << 4) | (1U << 5) | (1U << 6) | (1U << 7));

	// 配置速度（高速）
	GPIOC->OSPEEDR &= ~((3U << 8) | (3U << 10) | (3U << 12) | (3U << 14));
	GPIOC->OSPEEDR |= ((3U << 8) | (3U << 10) | (3U << 12) | (3U << 14));

	// 无上下拉
	GPIOC->PUPDR &= ~((3U << 8) | (3U << 10) | (3U << 12) | (3U << 14));

	// 初始状态关闭LED（高电平）
	GPIOC->ODR |= ((1U << 4) | (1U << 5) | (1U << 6) | (1U << 7));

}


//打开所有灯
void openAllLed(){
	//引用了自己的头文件当然可以使用定义的宏
	//delay_ms(50);
	LED1_ON;
	//beepInit();
	
	//delay_ms(50);
	LED2_ON;
	//beepInit();
	
	//delay_ms(50);
	LED3_ON;
	//beepInit();
	
	//delay_ms(50);
	LED4_ON;
	//beepInit();
	//delay_ms(50);
}

//关闭所有的灯
void closeAllLed(){
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	LED4_OFF;
}

//输入参数，单开一个灯
void ledX_on(u8 ledNum){
	//先关闭所有的灯
	closeAllLed();
	if(ledNum==1)
		LED1_ON;
	if(ledNum==2)
		LED2_ON;
	if(ledNum==3)
		LED3_ON;
	if(ledNum==4)
		LED4_ON;
}

//输入参数，单关闭一个灯
void ledX_off(u8 ledNum){
	//先关闭所有的灯
	closeAllLed();
	if(ledNum==1)
		LED1_OFF;
	if(ledNum==2)
		LED2_OFF;
	if(ledNum==3)
		LED3_OFF;
	if(ledNum==4)
		LED4_OFF;
}


//流水灯
void waterLed(u8 speed){
	static u32 time = 0;
	static u8 ledNum = 1;
	time++;
	ledX_on(ledNum);
	if(time >= (speed*50000)){
		ledX_off(ledNum);
		ledNum++;
		if(ledNum>4){
		ledNum = 1;
		}
		time = 0;
	}
}


