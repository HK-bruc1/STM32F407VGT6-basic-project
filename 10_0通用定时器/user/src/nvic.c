#include "nvic.h"
#include "stm32f4xx.h"                  // Device header
#include "selfprintf.h"
#include "systick.h"
#include "wang_time.h"
#include "LED.h"


//定义一个结构体变量并自动初始化
USART1BAL_t usart1_val;

//把单独一个标识扩展到全局,以便住主函数判断
volatile u8 countFlag;
//全局计数，中断尽量短而快
volatile u32 count;

//以1ms中断一次，用数组作事件单独判断
volatile u32 sysTickCount[5];

//以1ms中断一次，用数组作事件单独判断
volatile u32 TIM7Count[5];

//以1ms中断一次，用数组作事件单独判断
volatile u32 TIM6Count[5];



//串口1接收完成中断处理函数
void USART1_IRQHandler(void) {
    //USART1 中断处理函数
		//接收一个字节数据完成触发中断
    if (USART1->SR & (1U<<5)) {
        usart1_val.usart1_buff[usart1_val.length] = USART1->DR; //通过对 USART_DR 寄存器执行读入操作将该中断状态位清零
				usart1_val.length++;
    }
		
		//当传输多个字节数据后，线路空闲时触发中断
		//该位由软件序列清零（读入 USART_SR 寄存器，然后读入 USART_DR 寄存器）
		if(USART1->SR & (1U<<4)){
				//清除标志位
				//USART1->SR;
				USART1->DR;
				//在这里把缓冲区数组最后面加一个'\0'
				usart1_val.usart1_buff[usart1_val.length] = '\0';
				//重置缓冲区下标
				usart1_val.length = 0;
				//会自动检测尾巴0结束符号
				printf("usart1_buff:\t%s\r\n",usart1_val.usart1_buff);
				//给一个字符串接收完成的标识，定义全局变量，用于外部判断
				usart1_val.usart1StrRec_flag = 1;
		}
}


//EXTI8中断服务函数
void EXTI9_5_IRQHandler(void){
	//给一个临时静态局部变量用于计数
	//static u32 count = 0;
	//做一个标识，用作消抖使用
	static u8 extiFlag = 1;
	
	//共用的线，先判断具体是哪一个中断信号
	if(EXTI->PR & (1<<8)){
		//先清除状态位,置1清除状态
		EXTI->PR |= (1<<8);
		//按下和松开都会进入中断
		delay_ms(15);
		//直接访问寄存器快得多，不想用函数了。。。
		if((!(GPIOC->IDR & (1<<8))) && extiFlag){
			//紧急事件
			LED1_TURN;
			count++;
			//printf("物件数量:%d\r\n",count);
			//标识位锁住，在松开之前，紧急事件不会执行第二次
			extiFlag = 0;
			countFlag = 1;
		}else if((GPIOC->IDR & (1<<8)) && extiFlag==0){
			//按住进来一次，状态被清除，除开机械抖动不会在进来
			//15ms后如果机械抖动再进来，标识位锁住了，直接出去（小概率）
			//松开时会进来一次，把标识位解锁
			extiFlag = 1;
			//15ms后如果机械抖动,也进不来，条件不满足
		}
	}
}


//滴答定时中断服务函数(虽然相当于独立的，但是还是写在一起)
void SysTick_Handler(void){
	//清空当前值寄存器,清零对应状态位
	//不想外部知道就不用清零
	SysTick->VAL = 0;
	//打印用
	sysTickCount[0]++;
	//延时函数用，无符号int类型的数据，减到0之后，会环形结构，不会出现溢出的未定义行为
	sysTickCount[1]--;
	
	if(sysTickCount[0]==500){
		//计数器递减到0就触发一次中断
		printf("系统滴答定时中断测试\r\n");
		sysTickCount[0]=0;
	}
}


//基本定时器TIM6的中断服务函数
void TIM6_DAC_IRQHandler(void){
	//共享的中断通道先判断是什么中断信号
	if(TIM6->SR & (1U<<0)){
		//清除标志位,这里进入中断需要这个标志位，所以必须要清0
		//和系统滴答定时器不一样
		TIM6->SR &= ~(1U<<0);
		
		//打印用
		TIM6Count[0]++;
		//延迟函数用
		TIM6Count[1]--;
		
		if(TIM6Count[0]==3000){
			printf("基本定时器6定时中断测试\r\n");
			TIM6Count[0]=0;
		}
		
	}
	
}


//基本定时器TIM7的中断服务函数
void TIM7_IRQHandler(void){
	if(TIM7->SR & (1U<<0)){
		//清除标志位,这里进入中断需要这个标志位，所以必须要清0
		//和系统滴答定时器不一样
		TIM7->SR &= ~(1U<<0);
		
		
		//用于led呼吸的标志位
		static  u8 ledBreathFlag = 1; 
		
		//打印用
		TIM7Count[0]++;
		//LED3呼吸灯用
		TIM7Count[1]++;
		
		if(TIM7Count[0]==3000){
			printf("基本定时器7定时中断测试\r\n");
			TIM7Count[0]=0;
		}
		
		//LED3呼吸灯
		if(TIM7Count[1]==2){
			if(ledBreathFlag==1){
				TIM3->CCR1++;
				if(TIM3->CCR1 >= 1000){
					ledBreathFlag = 0;
				}
			}else if(ledBreathFlag==0){
				TIM3->CCR1--;
				if(TIM3->CCR1 < 1){
					ledBreathFlag = 1;
				}
			}
			//每一个亮度维持3ms
			TIM7Count[1]=0;
		}
	}
}

