//ʹ�������Ҫ�õ�����1����Ӧ�����úͳ�ʼ������Ҫ��

#include "selfprintf.h"

#pragma import(__use_no_semihosting)             

struct __FILE 
{ 
    int handle; 
}; 

FILE __stdout;       

//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
    x = x; 
} 

//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
    while((USART1->SR&(1<<6))==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
    return ch;
}