#include "exti.h"
#include "stm32f4xx.h"                  // Device header
#include "wang_time.h"
#include "LED.h"
#include "selfprintf.h"


//ʹPC8���ӵ�EXTI8,�����ⲿ�ж�����
void exti8Init(){
	/*IO����������*/
		//�˿�ʱ��ʹ�� PC8
		RCC->AHB1ENR |= (1U<<2);
		//�˿�ģʽ���� ���GPIO�ڵĸߵ͵�ƽ ͨ������
		GPIOC->MODER &= ~(3U<<16);
		//������
		GPIOC->PUPDR &= ~(3U<<16);
	
	
	/*EXTI����������*/
		//ϵͳ���ÿ�����ʱ��ʹ��------ʹ�üĴ���֮ǰҪ��ʹ�ܣ���Ȼ���ò���Ч����EXTI������û�����Բ������ã������������õ�
		RCC->APB2ENR |= (1<<14);
		//����Ҫ�õ�IO��ӳ�䵽��Ӧ��EXTI8�ӿ���
		SYSCFG->EXTICR[2] &= ~(15U<<0);
		SYSCFG->EXTICR[2] |= (1U<<1);
		//������/�½��ش��� �������£��Ǹߵ�ƽ��������,�ɿ�Ҳ��
		EXTI->RTSR |= (1<<8);
		EXTI->FTSR |= (1<<8);
		//�ж����μĴ���------�൱���ж�ʹ��
		EXTI->IMR |= (1<<8);

	/*NVIC����������*/
		// 1. �������ȼ�����Ϊ���飨ֻ��һ����������������
    //NVIC_SetPriorityGrouping(5); // ��ռ���ȼ� 2 λ����Ӧ���ȼ� 2 λ

    // 2. �������ȼ�����ֵ
    u32 pri = NVIC_EncodePriority(5, 1, 2); // ����ֵ5����ռ���ȼ� 1����Ӧ���ȼ� 2

    // 3. ���õ�������ж�Դ EXTI9_5_IRQHandler �����ȼ�
    NVIC_SetPriority(EXTI9_5_IRQn, pri);

    // 4. ���� EXTI0_IRQn �ж�
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}
