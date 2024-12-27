#include "redThink.h"
#include "stm32f4xx.h"                  // Device header
#include "wang_time.h"

//初始化PC8端口的状态
void redInit(){
	//总线对应的时钟使能打开
	RCC->AHB1ENR |= (1<<2);

	//清空第16和17位，置0,其他位不动
	GPIOC->MODER &= ~(3U<<16);

	//清空第16和17位，置0,其他位不动
	GPIOC->PUPDR &= ~(3U<<16);
}

//获取红外数据
u8 irData(){
	if(GPIOC->IDR & (1<<8)){
		//没有障碍物是高电平，if中非0
		return IR_NO;
	}
	return IR_OK;
}

//红外作为按键的扫描（不是实体的按键了，也要消抖）
u8 irKey_scan(void){
	//拿红外数据
	u8 irKey = irData();
	
	u8 key_val = IR_NO;//初始没有按下
	static u8 key_flag = 1; // 开锁
	
	// 同时检测四个按键和锁定状态
	if((irData() == IRKEY_PRESS) && key_flag)  // 有一个按键按下且没锁
	{
		delay_ms(20);  // 消抖
		if((irData() == IRKEY_PRESS))  // 确认按下是哪个键？
		{
			key_val = IRKEY_PRESS;
			//锁住
			key_flag = 0;
		}
	}
	
	//检测按键抬起
	if(irData() == IRKEY_UNPRESS){//一次只能检测一次按键操作
		//解锁
		key_flag = 1;
	}
	return key_val;  //返回键值
}
