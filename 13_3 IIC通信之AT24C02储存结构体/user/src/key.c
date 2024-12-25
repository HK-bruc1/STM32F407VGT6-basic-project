#include "stm32f4xx.h"                  // Device header
#include "LED.h"
#include "key.h"
#include "BEEP.h"
#include "wang_time.h"

//对应IO口的寄存器初始化
void keyInit(){
	//PA0
	//开对应总线的时钟使能
	RCC->AHB1ENR |= 1U;

	//清空第0和1位，置0,其他位不动
	GPIOA->MODER &= ~(3U);

	//清空第0和1位，置0,其他位不动
	GPIOA->PUPDR &= ~(3U);
	
	
	//PE2,3,4
	//开对应总线的时钟使能
	RCC->AHB1ENR |= (1U<<4);

	//清空第2和3位，置0,其他位不动
	GPIOE->MODER &= ~((3U<<4)|(3U<<6)|(3U<<8));

	//清空第0和1位，置0,其他位不动
	GPIOE->PUPDR &= ~((3U<<4)|(3U<<6)|(3U<<8));
}


// 按键检测函数
u8 key_scan(void)
{
    static u8 key_state = KEY_UNPRESS;  // 上次按键状态记录
    u8 current_key = KEY_UNPRESS;      // 当前按下的按键状态

    // 检测按键
    if (KEY1 == 1) current_key = KEY1_PRESS;
    else if (KEY2 == 1) current_key = KEY2_PRESS;
    else if (KEY3 == 1) current_key = KEY3_PRESS;
    else if (KEY4 == 1) current_key = KEY4_PRESS;

    if (current_key != KEY_UNPRESS) // 检测到按键按下
    {
        if (key_state == KEY_UNPRESS) // 之前未按下，当前是第一次按下
        {
            delay_ms(20); // 消抖
            if (current_key != KEY_UNPRESS) // 确认按下状态
            {
                key_state = current_key; // 更新按键状态
                return current_key;      // 返回当前按键值
            }
        }
    }
    else 
    {
        if (key_state != KEY_UNPRESS) // 检测到松开
        {
            //delay_ms(20); // 松开消抖,没按也会消抖，耽误时间
            if (KEY1 == 0 && KEY2 == 0 && KEY3 == 0 && KEY4 == 0) // 确认松开
            {
                key_state = KEY_UNPRESS; // 恢复未按下状态
            }
        }
    }
    return KEY_UNPRESS; // 返回未按下状态
}
