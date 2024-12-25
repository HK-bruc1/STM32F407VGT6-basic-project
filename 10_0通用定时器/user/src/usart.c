#include "usart.h"
#include "stm32f4xx.h"                  // Device header
#include "LED.h"
#include "key.h"
#include "string.h"
#include "nvic.h"
#include "BEEP.h"



//串口1发送接收初始化
void usart1Init(u32 baud){
	/*IO口控制器配置*/
  //端口时钟使能
	RCC->AHB1ENR |= (1U<<0); 
	
	//端口模式配置-------复用 PA9：发送 PA10：接收
	GPIOA->MODER &= ~(3U<<18 | 3u<<20);//先清零
	GPIOA->MODER |= (2U<<18 | 2U<<20);//再置1
	
	//输出类型
	GPIOA->OTYPER &= ~(1U<<9); //清零顺便置0
	
	//输出速度,慢速
	GPIOA->OSPEEDR &= ~(3U<<18);
	
	//上下拉
	GPIOA->PUPDR &= ~((3U<<18) | (3u<<20));//先清零顺便置0
	
	//复用功能寄存器配置
	//GPIOA->AFR[1] |= ~(7U<<4 | 7U<<8);//置0111，写错了！！！直接STLink断了
	GPIOA->AFR[1] |= (7U<<4 | 7U<<8);//置0111
	
	
	/*某个串口控制器配置*/
	//串口控制器时钟使能	
	RCC->APB2ENR |= (1U<<4);
	
	
	//CR1
	USART1->CR1 &= ~(1U<<15);//清零顺便置0 16倍采样
	USART1->CR1 &= ~(1U<<12);//清零顺便置0 字长
	USART1->CR1 |= ((1U<<2) | (1U<<3));//再置1
	
	//发送不用中断不用配置
	//启用接收非空中断,我判断是否接收完成使用SR中的RXNE：第五位，其对应中断也在第五位
	USART1->CR1 |= 1U<<5;
	//空闲中断使能也打开
	USART1->CR1 |= 1U<<4;
	
	
	//CR2
	USART1->CR2 &= ~(3U<<12);//清零顺便置0
	
	//BRR
	USART1->BRR = 84000000/baud;//赋值直接给
	
	//串口控制器使能
	USART1->CR1 |= (1U<<13);//串口控制器使能
}


//串口1中断的NVIC配置初始化
void USART1_NVIC_init(void) {
    // 1. 设置优先级分组为分组（只有一个，放在主函数）
    //NVIC_SetPriorityGrouping(5); // 抢占优先级 2 位，响应优先级 2 位

    // 2. 计算优先级编码值
    u32 pri = NVIC_EncodePriority(5, 1, 2); // 分组值5，抢占优先级 1，响应优先级 2

    // 3. 设置到具体的中断源 USART1 的优先级
    NVIC_SetPriority(USART1_IRQn, pri);

    // 4. 启用 USART1 中断
    NVIC_EnableIRQ(USART1_IRQn);
}


//串口1发送一个字节数据,接收字节使用接收中断了
void usart1_send_byte(u8 data){
	  //等待之前的数据发送完成（等待状态寄存器的发送完成位置1）
		while(!(USART1->SR & (1U<<6)));
    
    // 直接发送数据
    USART1->DR = data;
}


//串口1接收字符串控制流水灯
u8 usart1WaterLed(void){
	if(usart1_val.usart1StrRec_flag == 1){
		//字符串接收状态先重置
		usart1_val.usart1StrRec_flag = 0;
		if(strcmp((char *)usart1_val.usart1_buff,"open") == 0){
			return 11;
		}else if(strcmp((char *)usart1_val.usart1_buff,"close") == 0){
				return 12;
			}else if(strcmp((char *)usart1_val.usart1_buff,"speed1") == 0){
					return 1;
				}else if(strcmp((char *)usart1_val.usart1_buff,"speed2") == 0){
						return 2;
					}else if(strcmp((char *)usart1_val.usart1_buff,"speed3") == 0){
							return 3;
						}else if(strcmp((char *)usart1_val.usart1_buff,"speed4") == 0){
								return 4;
							}else if(strcmp((char *)usart1_val.usart1_buff,"speed5") == 0){
									return 5;
								}else if(strcmp((char *)usart1_val.usart1_buff,"speed6") == 0){
										return 6;
									}else if(strcmp((char *)usart1_val.usart1_buff,"speed7") == 0){
											return 7;
									}else if(strcmp((char *)usart1_val.usart1_buff,"speed8") == 0){
												return 8;
											}else if(strcmp((char *)usart1_val.usart1_buff,"speed9") == 0){
													return 9;
												}
	}
	
	//没有接收中断也返回值，别卡在这里
	return 100;
}


