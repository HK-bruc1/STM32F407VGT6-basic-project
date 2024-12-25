//使用这个需要用到串口1，对应的设置和初始化必须要有

#include "selfprintf.h"

#pragma import(__use_no_semihosting)             

struct __FILE 
{ 
    int handle; 
}; 

FILE __stdout;       

//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
    x = x; 
} 

//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
    while((USART1->SR&(1<<6))==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
    return ch;
}