#ifndef ADC_H
#define ADC_H
#include "stm32f4xx.h"                  // Device header

//����ͷ�ļ��ж����ˣ������Ϳɼ������ֻ����������Ļ���Ҫ��extern��ȫ�ֱ�����
//���ڴ�������
typedef struct {
	//��������������
	u16 lightData;
	//����������
	u16 gasData;
	//�¶�����
	float temp;
}ADC1_t;


void adc1_PC2_ch12_init(void);
u16 adc1_PC2_ch12_get(void);
void adc2_PC0_ch10_init(void);
u16 adc2_PC0_ch10_get(void);
void adc1_PC2PC0_ch12ch10ch16_init(void);
ADC1_t adc1_PC2PC0_ch12ch10ch16_get(void);
void adc1_interrupt_init(void);

#endif