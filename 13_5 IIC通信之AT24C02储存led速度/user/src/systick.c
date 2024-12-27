#include "systick.h"
#include "stm32f4xx.h"                  // Device header
#include "selfprintf.h"                  // Device header


//ϵͳ�δ�ʱ���жϳ�ʼ������
void sysTickInterrupt_init(u16 ms){
	//ʱ��Դѡ��
	SysTick->CTRL &= ~(1U<<2);
	
	//д������ֵ
	SysTick->LOAD = 21000 * ms - 1;
	
	//��յ�ǰֵ�Ĵ���
	SysTick->VAL = 0;
	
	//����ϵͳ�δ�ʱ�����ж�ʹ��
	SysTick->CTRL |= (1U<<1);
	
	/*NVIC����������*/
		//���ȼ�����(�����������г�ʼ����ǰ��)
		//�������ֵ
		u32 pri = NVIC_EncodePriority(5, 1, 2);
		//���þ����ж�Դ
		NVIC_SetPriority(SysTick_IRQn, pri);
		//���ö�Ӧͨ��ʹ���ж�,û�ж�Ӧͨ��������ʹ��
		//NVIC_EnableIRQ(SysTick_IRQn);
	
	//ʹ�ܶ�ʱ��
	SysTick->CTRL |= (1U<<0);
}