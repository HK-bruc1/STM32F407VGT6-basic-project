#include "GeneralTim.h"
#include "stm32f4xx.h"                  // Device header
#include "key.h"
#include "wang_time.h"
#include "selfprintf.h"
#include "redThink.h"
#include "nvic.h"


void tim3_led3_Init(void){
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
	//printf("tim3_led3初始化成功\r\n");
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

//控制舵机的初始化定时器14
void tim14_ServoInit(void){
	/*IO控制器配置*/
	//端口时钟使能 PA7
	RCC->AHB1ENR |= (1U<<0);
	//端口模式配置-------复用模式
	GPIOA->MODER &= ~(3U<<14);
	GPIOA->MODER |= (1U<<15);
	//输出类型
	GPIOA->OTYPER &= ~(1U<<7);
	//输出速度
	GPIOA->OSPEEDR &= ~(3U<<14);
	GPIOA->OSPEEDR |= (1U<<15);
	//上下拉
	GPIOA->PUPDR &= ~(3U<<14);
	//复用功能寄存器
	GPIOA->AFR[0] &= ~(15U<<28);
	GPIOA->AFR[0] |= (9U<<28);
	
	/*通用定时器控制器配置*/
	//定时器时钟使能
	RCC->APB1ENR |= (1U<<8);
	//CR1
	//各影子寄存器
	TIM14->CR1 |= (1U<<7);
	//边沿对齐
	TIM14->CR1 &= ~(3U<<5);
	TIM14->CR1 &= ~(1U<<4);
	TIM14->CR1 &= ~(1U<<3);
	TIM14->CR1 &= ~(1U<<1);
	
	//SMCR
	//时钟源来自内部时钟
	TIM14->SMCR &= ~(7U<<0);
	
	//CCMRx 
	TIM14->CCMR1 &= ~(3U<<0);
	TIM14->CCMR1 |= (1U<<3);
	TIM14->CCMR1 &= ~(7U<<4);
	TIM14->CCMR1 |= (3U<<5);
	
	//CCER
	TIM14->CCER &= ~(1U<<3);
	//舵机高电平为有效，所以设置高电平
	TIM14->CCER &= ~(1U<<1);
	//设置定时器3的预分频器（PSC）为1679（因为分频系数=PSC+1）
	TIM14->PSC = 1679;
	//ARR
	TIM14->ARR = 999;
	//CCRx
	//初始0度
	TIM14->CCR1 = 25;
	//EGR 手动生成更新事件，装载值
	TIM14->EGR |= (1U<<0); 
	
	//tim14通道1输出使能
	TIM14->CCER |= (1U<<0);
	//计数器使能
	TIM14->CR1 |= (1U<<0);
	//printf("舵机初始化完成");
}

//按键控制舵机的角度,初始CCR1值为25（对应0度）定时器14
void tim14KeyServo(void){
	//记住上一次的值
	static u16 currentAngle;
	u8 keyVal = 0;

	keyVal = key_scan();

	switch(keyVal)
	{
			case 1: // 假设这是设置到180度的按键
					printf("设置到180度\r\n");
					currentAngle = 125;
					TIM14->CCR1 = currentAngle;
					break;
			case 2: // 假设这是增加角度的按键
					printf("增加角度\r\n");
					if(currentAngle < 125) // 确保不超过最大值（180度）
					{
							currentAngle += 5; // 每次增加5，意味着角度增加15度
					}
					TIM14->CCR1 = currentAngle; // 更新PWM占空比
					break;
			case 3: // 假设这是设置到0度的按键
					printf("设置到0度\r\n");
					currentAngle = 25;
					TIM14->CCR1 = currentAngle;
					break;
			case 4: // 假设这是减少角度的按键
					printf("减少角度\r\n");
					if(currentAngle > 25) // 确保不低于最小值（0度）
					{
							currentAngle -= 5; // 每次减少5，意味着角度减少15度
					}
					TIM14->CCR1 = currentAngle; // 更新PWM占空比
					break;
			default:
					// 无操作
					break;
	}
}

//定时器13电机初始化
void tim13_MotorInit(void){
	/*IO控制器配置*/
	//端口时钟使能 PA6
	RCC->AHB1ENR |= (1U<<0);
	//端口模式配置-------复用模式
	GPIOA->MODER &= ~(3U<<12);
	GPIOA->MODER |= (1U<<13);
	//输出类型
	GPIOA->OTYPER &= ~(1U<<6);
	//输出速度
	GPIOA->OSPEEDR &= ~(3U<<12);
	GPIOA->OSPEEDR |= (1U<<13);
	//上下拉
	GPIOA->PUPDR &= ~(3U<<12);
	//复用功能寄存器
	GPIOA->AFR[0] &= ~(15U<<24);
	GPIOA->AFR[0] |= (9U<<24);
	
	/*通用定时器控制器配置*/
	//定时器时钟使能
	RCC->APB1ENR |= (1U<<7);
	//CR1
	//各影子寄存器
	TIM13->CR1 |= (1U<<7);
	//边沿对齐
	TIM13->CR1 &= ~(3U<<5);
	TIM13->CR1 &= ~(1U<<4);
	TIM13->CR1 &= ~(1U<<3);
	TIM13->CR1 &= ~(1U<<1);
	
	//SMCR
	//时钟源来自内部时钟
	TIM13->SMCR &= ~(7U<<0);
	
	//CCMRx 
	TIM13->CCMR1 &= ~(3U<<0);
	TIM13->CCMR1 |= (1U<<3);
	TIM13->CCMR1 &= ~(7U<<4);
	TIM13->CCMR1 |= (3U<<5);
	
	//CCER
	TIM13->CCER &= ~(1U<<3);
	//舵机高电平为有效，所以设置高电平
	TIM13->CCER &= ~(1U<<1);
	//设置定时器13的预分频器（PSC）为839（因为分频系数=PSC+1）
	//分频后计数频率为100KHZ 100,000次/s  100次/ms 
	TIM13->PSC = 840-1;
	//ARR
	TIM13->ARR = 10-1;
	//CCRx
	//设定CCRx为0时，占空比为0%。
	TIM13->CCR1 = 0;
	//EGR 手动生成更新事件，装载值
	TIM13->EGR |= (1U<<0); 
	
	//tim13通道1输出使能
	TIM13->CCER |= (1U<<0);
	//计数器使能
	TIM13->CR1 |= (1U<<0);
	//printf("电机初始化完成");
}
//按键控制电机转速
void tim13KeyMotor(void){
    // 静态变量用于记住上一次的PWM值（即电机转速）
    static u16 currentSpeed;
		static u8 statuFlag = 0;
    u8 keyVal = 0;

    keyVal = key_scan();

		switch(keyVal)
    {
        case 1: // 启动电机
            printf("启动电机\r\n");
						if(statuFlag==0){
							currentSpeed = 1; // 设置一个基础转速，这里假设10%占空比为基础转速
							TIM13->CCR1 = currentSpeed;
						}else {
							TIM13->CCR1 = currentSpeed;
						}
            break;
        case 2: // 增加转速
            printf("增加转速\r\n");
            if(currentSpeed < 10) 
            {
                currentSpeed += 1; // 每次增加10%占空比，即1
            }
            TIM13->CCR1 = currentSpeed; 
            break;
        case 3: // 关闭电机
            printf("关闭电机\r\n");
            TIM13->CCR1 = 0;
						statuFlag = 1;
            break;
        case 4: // 减少转速
            printf("减少转速\r\n");
            if(currentSpeed > 1) 
            {
                currentSpeed -= 1; // 每次减少10%占空比，即10
            }
            TIM13->CCR1 = currentSpeed; 
            break;
        default:
            break;
    }
}

//红外感应电机启动和关闭
void tim13RedThinkMotor(void){
	//获取红外感应数据
	if(IR_statu == IR_OK){
		//打开电机的高速模式运行5s后关闭
		TIM13->CCR1 = 1;
		baseTim6DelayPlus_Ms(5000);
		TIM13->CCR1 = 0;
	}
}

//RGB模块TIM5初始化
void tim5_RGBInit(void){
	/*IO控制器配置*/
	//端口时钟使能 PA1,2,3
	RCC->AHB1ENR |= (1U<<0);
	//端口模式配置-------复用模式
	GPIOA->MODER &= ~((3U<<2) | (3U<<4) | (3U<<6));
	GPIOA->MODER |= ((1U<<3) | (1U<<5) | (1U<<7));
	//输出类型
	GPIOA->OTYPER &= ~((1U<<1) | (1U<<2) | (1U<<3));
	//输出速度
	GPIOA->OSPEEDR &= ~((3U<<2) | (3U<<4) | (3U<<6));
	GPIOA->OSPEEDR |= ((1U<<3) | (1U<<5) | (1U<<7));
	//上下拉
	GPIOA->PUPDR &= ~((1U<<1) | (1U<<2) | (1U<<3));
	//复用功能寄存器
	GPIOA->AFR[0] &= ~((15U<<4) | (15U<<8) | (15U<<12));
	GPIOA->AFR[0] |= ((2U<<4) | (2U<<8) | (2U<<12));
	
	/*通用定时器控制器配置*/ //通道2，3，4
	//定时器时钟使能
	RCC->APB1ENR |= (1U<<3);
	//CR1
	//各影子寄存器
	TIM5->CR1 |= (1U<<7);
	//边沿对齐
	TIM5->CR1 &= ~(3U<<5);
	TIM5->CR1 &= ~(1U<<4);
	TIM5->CR1 &= ~(1U<<3);
	TIM5->CR1 &= ~(1U<<1);
	
	//SMCR
	//时钟源来自内部时钟
	TIM5->SMCR &= ~(7U<<0);
	
	//CCMRx 开三个通道
	TIM5->CCMR1 &= ~(3U<<8);
	TIM5->CCMR1 |= (1U<<11);
	TIM5->CCMR1 &= ~(7U<<12);
	TIM5->CCMR1 |= (3U<<13);
	
	TIM5->CCMR2 &= ~((3U<<0) | (3U<<8));
	TIM5->CCMR2 |= ((1U<<3) | (1U<<11));
	TIM5->CCMR2 &= ~((7U<<4) | (7U<<12));
	TIM5->CCMR2 |= ((3U<<5) | (3U<<13));
	
	//CCER
	//rgb低电平为有效，所以设置低电平
	TIM5->CCER |= (1U<<5);
	TIM5->CCER |= (1U<<7);
	TIM5->CCER |= (1U<<9);
	TIM5->CCER |= (1U<<11);
	TIM5->CCER |= (1U<<13);
	TIM5->CCER |= (1U<<15);
	
	//设置定时器5的预分频器（PSC）为83（因为分频系数=PSC+1）
	//1 kHz的PWM频率
	TIM5->PSC = 83;
	//ARR
	TIM5->ARR = 999;
	//CCRx
	//初始占空比各50%
	//PA2 G CCR3    PA3 B CCR4     PA1 R CCR2 
	TIM5->CCR2 = 0;
	TIM5->CCR3 = 0;
	TIM5->CCR4 = 0;
	//EGR 手动生成更新事件，装载值
	TIM5->EGR |= (1U<<0); 
	
	//tim14通道2，3，4输出使能
	TIM5->CCER |= (1U<<4 | 1U<<8 | 1U<<12);
	//计数器使能
	TIM5->CR1 |= (1U<<0);
	//printf("RGB模块初始化完成");
}




//按键1的输入捕获
void tim5_KeyCapture(void){
	/*IO控制器配置*/
	//端口时钟使能 PA0
	RCC->AHB1ENR |= (1U<<0);
	//端口模式配置-------复用模式
	GPIOA->MODER &= ~(3U<<0);
	GPIOA->MODER |= (1U<<1);
	//输出类型
	GPIOA->OTYPER &= ~(1U<<0);
	//输出速度
	GPIOA->OSPEEDR &= ~(3U<<0);
	GPIOA->OSPEEDR |= (1U<<1);
	//上下拉
	GPIOA->PUPDR &= ~(1U<<0);
	//复用功能寄存器
	GPIOA->AFR[0] &= ~(15U<<0);
	GPIOA->AFR[0] |= (2U<<0);
	
	/*通用定时器控制器配置*/ //通道1
	//定时器时钟使能
	RCC->APB1ENR |= (1U<<3);
	//CR1
	//各影子寄存器
	TIM5->CR1 |= (1U<<7);
	//滤波器采样时钟不分频
	TIM5->CR1 &= ~(3U<<8);
	//边沿对齐
	TIM5->CR1 &= ~(3U<<5);
	TIM5->CR1 &= ~(1U<<4);
	TIM5->CR1 &= ~(1U<<3);
	TIM5->CR1 &= ~(1U<<2);
	TIM5->CR1 &= ~(1U<<1);
	
	
	//SMCR
	//时钟源来自内部时钟
	TIM5->SMCR &= ~(7U<<0);
	
	
	//CCMRx 
	TIM5->CCMR1 &= ~(3U<<0);
	TIM5->CCMR1 |= (1U<<0);
	TIM5->CCMR1 &= ~(3U<<2);
	TIM5->CCMR1 &= ~(15U<<4);
	TIM5->CCMR1 |= (15U<<4);
	
	
	
	//CCER 先设置上升沿触发
	TIM5->CCER &= ~(1U<<1);
	TIM5->CCER &= ~(1U<<3);
	
	//设置定时器5的预分频器（PSC）为83（因为分频系数=PSC+1）
	//计数频率为84000000/84 = 1000KHZ  1000次/ms  1次/us
	TIM5->PSC = 83;
	//ARR
	TIM5->ARR = 65535;

	//EGR 手动生成更新事件，装载值
	TIM5->EGR |= (1U<<0); 
	
	//tim5通道1输入使能
	TIM5->CCER |= (1U<<0);
	//DIER放后面 中断使能
	TIM5->DIER |= ((1U<<0) | (1U<<1));
	//计数器使能
	TIM5->CR1 |= (1U<<0);
	
	// 1. 设置优先级分组为分组（只有一个，放在主函数）
    //NVIC_SetPriorityGrouping(5); // 抢占优先级 2 位，响应优先级 2 位

    // 2. 计算优先级编码值
    u32 pri = NVIC_EncodePriority(5, 1, 1); // 分组值5，抢占优先级 1，响应优先级 2

    // 3. 设置到具体的中断源 TIM5_IRQn 的优先级
    NVIC_SetPriority(TIM5_IRQn, pri);

    // 4. 启用 TIM5_IRQn 中断
    NVIC_EnableIRQ(TIM5_IRQn);
		printf("输入捕获初始化完成");
}


//超声波的ECHO回响捕获（定时器4的初始化）
void tim4_SRECHO_CaptureInit(void){
	/*IO控制器配置*/
	//端口时钟使能 PB6
	RCC->AHB1ENR |= (1U<<1);
	//端口模式配置-------复用模式
	GPIOB->MODER &= ~(3U<<12);
	GPIOB->MODER |= (1U<<13);
	//上下拉
	GPIOB->PUPDR &= ~(1U<<12);
	//复用功能寄存器
	GPIOB->AFR[0] &= ~(15U<<24);
	GPIOB->AFR[0] |= (2U<<24);
	
	/*通用定时器控制器配置*/ //通道1
	//定时器时钟使能
	RCC->APB1ENR |= (1U<<2);
	//CR1
	//各影子寄存器
	TIM4->CR1 |= (1U<<7);
	//滤波器采样时钟不分频
	TIM4->CR1 &= ~(3U<<8);
	//边沿对齐
	TIM4->CR1 &= ~(3U<<5);
	TIM4->CR1 &= ~(1U<<4);
	TIM4->CR1 &= ~(1U<<3);
	TIM4->CR1 &= ~(1U<<2);
	TIM4->CR1 &= ~(1U<<1);
	
	
	//SMCR
	//时钟源来自内部时钟
	TIM4->SMCR &= ~(7U<<0);
	
	
	//CCMRx 
	TIM4->CCMR1 &= ~(3U<<0);
	TIM4->CCMR1 |= (1U<<0);
	TIM4->CCMR1 &= ~(3U<<2);
	TIM4->CCMR1 &= ~(15U<<4);
	TIM4->CCMR1 |= (15U<<4);
	
	
	
	//CCER 先设置上升沿触发
	TIM4->CCER &= ~(1U<<1);
	TIM4->CCER &= ~(1U<<3);
	
	//设置定时器4的预分频器（PSC）为83（因为分频系数=PSC+1）
	//计数频率为84000000/84 = 1000KHZ  1000次/ms  1次/us
	TIM4->PSC = 83;
	//ARR
	TIM4->ARR = 65535;

	//EGR 手动生成更新事件，装载值
	TIM4->EGR |= (1U<<0); 
	
	//tim4通道1输入使能
	TIM4->CCER |= (1U<<0);
	//DIER放后面 中断使能
	TIM4->DIER |= ((1U<<0) | (1U<<1));
	//计数器使能
	TIM4->CR1 |= (1U<<0);
	
	// 1. 设置优先级分组为分组（只有一个，放在主函数）
    //NVIC_SetPriorityGrouping(5); // 抢占优先级 2 位，响应优先级 2 位

    // 2. 计算优先级编码值
    u32 pri = NVIC_EncodePriority(5, 0, 0); // 分组值5，抢占优先级 1，响应优先级 2

    // 3. 设置到具体的中断源 TIM4_IRQn 的优先级
    NVIC_SetPriority(TIM4_IRQn, pri);

    // 4. 启用 TIM4_IRQn 中断
    NVIC_EnableIRQ(TIM4_IRQn);
		//printf("用于SR04回响的输入捕获初始化完成");
}


