#include "GeneralTim.h"
#include "stm32f4xx.h"                  // Device header
#include "key.h"
#include "wang_time.h"


void tim3Init(void){
	/*IO����������*/
	//�˿�ʱ��ʹ�� PC6
	RCC->AHB1ENR |= (1U<<2);
	//�˿�ģʽ����-------����ģʽ
	GPIOC->MODER &= ~(3U<<12);
	GPIOC->MODER |= (1U<<13);
	//�������
	GPIOC->OTYPER &= ~(1U<<6);
	//����ٶ�
	GPIOC->OSPEEDR &= ~(3U<<12);
	GPIOC->OSPEEDR |= (1U<<13);
	//������
	GPIOC->PUPDR &= ~(3U<<12);
	//���ù��ܼĴ���
	GPIOC->AFR[0] &= ~(15U<<24);
	GPIOC->AFR[0] |= (1U<<25);
	
	/*ͨ�ö�ʱ������������*/
	//��ʱ��ʱ��ʹ��
	RCC->APB1ENR |= (1U<<1);
	//CR1
	//��Ӱ�ӼĴ���
	TIM3->CR1 |= (1U<<7);
	//���ض���
	TIM3->CR1 &= ~(3U<<5);
	TIM3->CR1 &= ~(1U<<4);
	TIM3->CR1 &= ~(1U<<3);
	TIM3->CR1 &= ~(1U<<1);
	
	//SMCR
	//ʱ��Դ�����ڲ�ʱ��
	TIM3->SMCR &= ~(7U<<0);
	
	//CCMRx 
	TIM3->CCMR1 &= ~(3U<<0);
	TIM3->CCMR1 |= (1U<<3);
	TIM3->CCMR1 &= ~(7U<<4);
	TIM3->CCMR1 |= (3U<<5);
	
	//CCER
	TIM3->CCER &= ~(1U<<3);
	//LED�͵�ƽΪ��Ч���������õ͵�ƽ
	TIM3->CCER |= (1U<<1);
	//PSC 84000000MHZ 84000/ms---->1000MHZ 84��Ƶ 1ms 1000����
	TIM3->PSC = 83;
	//ARR
	TIM3->ARR = 1000;
	//CCRx
	TIM3->CCR1 = 1000;
	//EGR �ֶ����ɸ����¼���װ��ֵ
	TIM3->EGR |= (1U<<0); 
	
	//tim3ͨ��1���ʹ��
	TIM3->CCER |= (1U<<0);
	//������ʹ��
	TIM3->CR1 |= (1U<<0);
}


//ͨ��PWM��ռ�ձ�������LED3������
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

//ͨ�ö�ʱ��3 LED3������Ч�� ѭ������ռ�ձȵ�ֵ
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
