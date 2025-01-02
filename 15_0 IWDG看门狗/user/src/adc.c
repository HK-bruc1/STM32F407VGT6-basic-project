#include "adc.h"
#include "stm32f4xx.h"                  // Device header
#include "selfprintf.h"



//PC2��ӦADC1��ͨ��12��ʼ��
void adc1_PC2_ch12_init(void){
	//GPIO�ڳ�ʼ��PC2
		//�˿�ʱ��ʹ��
		RCC->AHB1ENR |= (1U<<2);
	
		GPIOC->MODER &= ~(3U<<4);
		GPIOC->MODER |= (3U<<4);
	
	//ADC����������
		//����ʱ��ʹ��
		RCC->APB2ENR |= (1U<<8);
		//CR1
		ADC1->CR1 &= ~(3U<<24);
		ADC1->CR1 &= ~(1U<<8);//������棬��ɨ��ģʽ
		//CR2
		ADC1->CR2 &= ~(1U<<11);
		ADC1->CR2 |= (1U<<10);
		ADC1->CR2 &= ~(1U<<1);
		//ADC1->CR2 |= (1U<<0);//������棬ʹ��ת����
		//SMPRX
		ADC1->SMPR1 &= ~(7U<<6);
		ADC1->SMPR1 |= (7U<<6);
		//SQRX
		ADC1->SQR1 &= ~(15U<<20); 
		ADC1->SQR3 &= ~(0x1f<<0);
		ADC1->SQR3 |= (12U<<0);
		//CCR
		ADC->CCR &= ~(3U<<16);
		
		//ADC1->CR1 &= ~(1U<<8);//������棬ʹ��ɨ��ģʽ
		ADC1->CR2 |= (1U<<0);//������棬ʹ��ת����
}

//��ȡͨ��12ת���������
u16 adc1_PC2_ch12_get(void){
	u16 data = 0;
	//����ת��
	ADC1->CR2 |= (1U<<30);
	//�ȴ�ת�����
	while(!(ADC1->SR & (1U<<1)));
	//��ȡ���ݲ�����˱�־λ
	data = ADC1->DR;
	return data;
}

//PC0��Ӧadc2_ch10�ĳ�ʼ��
void adc2_PC0_ch10_init(void){
	//GPIO�ڳ�ʼ��PC0
		//�˿�ʱ��ʹ��
		RCC->AHB1ENR |= (1U<<2);
	
		GPIOC->MODER &= ~(3U<<0);
		GPIOC->MODER |= (3U<<0);
	
	//ADC����������
		//����ʱ��ʹ��
		RCC->APB2ENR |= (1U<<9);
		//CR1
		ADC2->CR1 &= ~(3U<<24);
		//ADC2->CR1 &= ~(1U<<8);//������棬��ɨ��ģʽ
		//CR2
		ADC2->CR2 &= ~(1U<<11);
		ADC2->CR2 |= (1U<<10);
		ADC2->CR2 &= ~(1U<<1);
		//ADC2->CR2 |= (1U<<0);//������棬ʹ��ת����
		//SMPRX
		ADC2->SMPR1 &= ~(7U<<0);
		ADC2->SMPR1 |= (7U<<0);
		//SQRX
		ADC2->SQR1 &= ~(15U<<20); 
		ADC2->SQR3 &= ~(0x1f<<0);
		ADC2->SQR3 |= (10U<<0);
		//CCR
		ADC->CCR &= ~(3U<<16);
		
		ADC2->CR1 &= ~(1U<<8);//������棬ʹ��ɨ��ģʽ
		ADC2->CR2 |= (1U<<0);//������棬ʹ��ת����
}

//��ȡADC2ͨ��10ת���������
u16 adc2_PC0_ch10_get(void){
	u16 data = 0;
	//����ת��
	ADC2->CR2 |= (1U<<30);
	//�ȴ�ת�����
	while(!(ADC2->SR & (1U<<1)));
	//��ȡ���ݲ�����˱�־λ
	data = ADC2->DR;
	return data;
}


//ͬADC�Ķ�ͨ��ʹ�� ��һ���¶�ͨ����û��GPIO�ڣ�ͨ��16��
//PC0��Ӧadc1_ch10�ĳ�ʼ��
//PC2��ӦADC1��ͨ��12��ʼ��
void adc1_PC2PC0_ch12ch10ch16_init(void){
	//GPIO�ڳ�ʼ��PC0 PC2
		//�˿�ʱ��ʹ��
		RCC->AHB1ENR |= (1U<<2);
	
		GPIOC->MODER &= ~(3U<<0 | 3U<<4);
		GPIOC->MODER |= (3U<<0 | 3U<<4);
	
	//ADC����������
		//����ʱ��ʹ��
		RCC->APB2ENR |= (1U<<8);
		//CR1
		ADC1->CR1 &= ~(3U<<24);
		ADC1->CR1 |= (1U<<8);//ɨ��ģʽ
	
		//CR2
		ADC1->CR2 &= ~(1U<<11);
		ADC1->CR2 |= (1U<<10);
		ADC1->CR2 &= ~(1U<<1);
		//ADC2->CR2 |= (1U<<0);//������棬ʹ��ת����
	
		//SMPRX����ͨ����
		ADC1->SMPR1 &= ~(7U<<0 | 7U<<6 | 7U<<18);
		ADC1->SMPR1 |= (7U<<0 | 7U<<6 | 7U<<18);
		
		//SQRX
		ADC1->SQR1 &= ~(15U<<20);
		ADC1->SQR1 |= (2U<<20);	//�ܼ�����ת��	
		
		ADC1->SQR3 &= ~(0x1f<<0);
		ADC1->SQR3 |= (12U<<0);  //��ת��12ͨ��������
		
		ADC1->SQR3 &= ~(0x1f<<5);
		ADC1->SQR3 |= (10U<<5);  //��ת��10ͨ��������
		
		ADC1->SQR3 &= ~(0x1f<<10);
		ADC1->SQR3 |= (16U<<10);  //��ת��16ͨ�����¶�
		
		//CCR
		ADC->CCR &= ~(3U<<16);
		ADC->CCR |= (1U<<23);//�����¶�ͨ��
		
		ADC1->CR2 |= (1U<<0);//������棬ʹ��ת����
		//printf("adc1_PC2PC0_ch12ch10_init��ʼ�����\r\n");
}

//��ȡADC1ͨ��10 12ת����������Լ��¶�
ADC1_t adc1_PC2PC0_ch12ch10ch16_get(void){
	ADC1_t adc1_data;
	//����ת��
	ADC1->CR2 |= (1U<<30);
	//�ȴ�ת�����
	while(!(ADC1->SR & (1U<<1)));
	//�Ȼ�ȡ��������
	adc1_data.lightData = ADC1->DR;
	adc1_data.lightData = (4095-adc1_data.lightData)/4095.0*100;
	
	//�ȴ�ת�����
	while(!(ADC1->SR & (1U<<1)));
	//��ȡ��������
	adc1_data.gasData = ADC1->DR;
	adc1_data.gasData = adc1_data.gasData/4095.0*100;
	
	//�ȴ�ת�����
	while(!(ADC1->SR & (1U<<1)));
	//��ȡ�¶�����
	adc1_data.temp = ADC1->DR;
	adc1_data.temp = (adc1_data.temp*0.805-760)/2.5+25;
	
//	printf("���ص�����ͨ��ת��������/r/n");
//	printf("%d %d %f\r\n",adc1_data.lightData,adc1_data.gasData,adc1_data.temp);
	return adc1_data;
}

//ʹ���жϷ�ʽ��Ϊ��ͨ��ת�����ݻ�ȡʱ��(�����ǲ�ѯ�ȴ�)
void adc1_interrupt_init(void){
	//GPIO�ڳ�ʼ��PC0 PC2
		//�˿�ʱ��ʹ��
		RCC->AHB1ENR |= (1U<<2);
	
		GPIOC->MODER &= ~(3U<<0 | 3U<<4);
		GPIOC->MODER |= (3U<<0 | 3U<<4);
	
	//ADC����������
		//����ʱ��ʹ��
		RCC->APB2ENR |= (1U<<8);
		//CR1
		ADC1->CR1 &= ~(3U<<24);
		ADC1->CR1 |= (1U<<8);//ɨ��ģʽ
		//ADC1->CR1 |= (1U<<5);//ת����ʶ��1ʱ�����ж�
	
		//CR2
		ADC1->CR2 &= ~(1U<<11);
		ADC1->CR2 |= (1U<<10);
		ADC1->CR2 &= ~(1U<<1);
		//ADC2->CR2 |= (1U<<0);//������棬ʹ��ת����
	
		//SMPRX����ͨ����
		ADC1->SMPR1 &= ~(7U<<0 | 7U<<6 | 7U<<18);
		ADC1->SMPR1 |= (7U<<0 | 7U<<6 | 7U<<18);
		
		//SQRX
		ADC1->SQR1 &= ~(15U<<20);
		ADC1->SQR1 |= (2U<<20);	//�ܼ�����ת��	
		
		ADC1->SQR3 &= ~(0x1f<<0);
		ADC1->SQR3 |= (12U<<0);  //��ת��12ͨ��������
		
		ADC1->SQR3 &= ~(0x1f<<5);
		ADC1->SQR3 |= (10U<<5);  //��ת��10ͨ��������
		
		ADC1->SQR3 &= ~(0x1f<<10);
		ADC1->SQR3 |= (16U<<10);  //��ת��16ͨ�����¶�
		
		//CCR
		ADC->CCR &= ~(3U<<16);
		ADC->CCR |= (1U<<23);//�����¶�ͨ��
		
		//Ƭ�����軹��ҪNVIC�������ʹ���ж�
		// 1. �������ȼ�����Ϊ���飨ֻ��һ����������������
		//NVIC_SetPriorityGrouping(5); // ��ռ���ȼ� 2 λ����Ӧ���ȼ� 2 λ

		// 2. �������ȼ�����ֵ
		u32 pri = NVIC_EncodePriority(5, 0, 0); // ����ֵ5����ռ���ȼ� 0����Ӧ���ȼ� 0

		// 3. ���õ�������ж�Դ ADC_IRQn �����ȼ�
		//������ж�ͨ�����ǵ����жϷ��������ж�һ����ʲô�ж��ź�
		NVIC_SetPriority(ADC_IRQn, pri);

		// 4. ���� ADC_IRQn �жϣ�����ADC����������
		NVIC_EnableIRQ(ADC_IRQn);
		
		ADC1->CR1 |= (1U<<5);//ת����ʶ��1ʱ�����ж�
		ADC1->CR2 |= (1U<<0);//������棬ʹ��ת����
		//printf("��ͨ���ж����ó�ʼ�����\r\n");
}