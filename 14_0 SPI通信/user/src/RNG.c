#include "RNG.h"
#include "stm32f4xx.h"                  // Device header

//��ʼ��
void RNG_Init(void) {
    // ����RNGʱ��
    RCC->AHB2ENR |= (1<<6);  // ENG ʱ�ӿ���
    
    // ����RNG
    RNG->CR |= (1<<2);       // �����ʹ�ܿ���
}

//���������,�ȳ�ʼ����ʹ��
u32 getRandomNumber(void) {
    u32 randnum;
		while(!(RNG->SR&(1<<0)));	//�ȴ����������  
		randnum = RNG->DR;//��������ȡģ��������������ȶ���
		return randnum ;
}

