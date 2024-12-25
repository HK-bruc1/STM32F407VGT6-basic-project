#include "functions.h"
#include "adc.h"
#include "GeneralTim.h"
#include "wang_time.h"
#include "selfprintf.h"
#include "nvic.h"
#include "key.h"
#include "RNG.h"
#include "event_handler.h"
#include "at24c02.h"

//��Դ�ļ����ڶ����ۺ��Ժ����������������ĳ�ʼ�����ã�����ʱ�������úõ�


volatile uint8_t randomColorFlag = 0;


// ��װAT24C02д��ṹ������ĺ���������ṹ��ָ�뼴�ɣ����⸱���˷ѿռ�
void writeBookToEEPROM(u8 startAddress, const Book* book) {
	//��ַΪ0~255��u8�㹻
	u8 address = startAddress;
	
	// д��name,����ṹ���Ա���ֽڴ�С�����д��
	//��ҳд����ֽ�,�ڲ���ʼ��ַ����Ҫд�����ݵĵ�ַ��д���ֽڵ�����
	//book->name ��ͬ�� (&book->name[0])���� name �������Ԫ�صĵ�ַ
	//u8 name[30]��char *���Ͳ�ƥ�䣬��Ҫǿת��
	at24c02_skip_page_write(address,(char *)book->name,sizeof(book->name));
	
	//��һ��д����ڲ���ַΪ
	address+=sizeof(book->name);
	
	//д��������
	at24c02_skip_page_write(address,(char *)book->writer,sizeof(book->writer));
	
	//��һ��д����ڲ���ַΪ
	address+=sizeof(book->writer);
	
	//д����
	at24c02_skip_page_write(address,(char *)book->number,sizeof(book->number));
	
	//��һ��д����ڲ���ַΪ
	address+=sizeof(book->number);
	
	//д���ȶ�,����ֱ��д�뵥���ֽھͿ��ԣ��۸񵥶����룿���ַ���д�룿����





}


//��������RGB��ɫ,�ȳ�ʼ��tim5_RGBInit����keyInit();
void tim5_Key_RGB(void){
	u8 keyVal = 0;
	keyVal = key_scan();
	switch(keyVal){
		//��ɫ
		case 1:
			TIM5->CCR2 = 0;
			TIM5->CCR3 = 0;
			TIM5->CCR4 = 999;
			break;
		//��ɫ
		case 2:
			TIM5->CCR2 = 0;
			TIM5->CCR3 = 999;
			TIM5->CCR4 = 0;
			break;
		//��ɫ
		case 3:
			TIM5->CCR2 = 999;
			TIM5->CCR3 = 0;
			TIM5->CCR4 = 0;
			break;
		case 4:
			//���������ɫģʽ
			randomColorFlag = !randomColorFlag;
			if(!randomColorFlag) {
				TIM5->CCR2 = 0; // ֹͣ�����ɫʱ������ѡ��һ��Ĭ����ɫ��ر�LED
				TIM5->CCR3 = 0;
				TIM5->CCR4 = 0;
      }
			break;
		default:
			break;
	}
}

//RGB�����ɫ����
void changeColorRandomly(void) {
    u16 red = getRandomNumber() % 1000;  // ȷ��ֵ��0��999֮��
    u16 green = getRandomNumber() % 1000;
    u16 blue = getRandomNumber() % 1000;
    
    TIM5->CCR2 = red;
    TIM5->CCR3 = green;
    TIM5->CCR4 = blue;
}



//��������ʼ��
void SR04_Init(void){
	
	//����ECHO����Ķ�ʱ��4�ĳ�ʼ��
	tim4_SRECHO_CaptureInit();
	
	//���ڷ���ߵ�ƽ��PB8�ĳ�ʼ��
	GPIOB->MODER &= ~(3U<<16);
	GPIOB->MODER |= (1U<<16);
	
	GPIOB->OTYPER |= (1U<<8);
	
	GPIOB->OSPEEDR &= ~(3U<<16);
	GPIOB->OSPEEDR |= (1U<<17);
	
	GPIOB->PUPDR &= ~(3U<<16);
	//��Ĭ�ϵ͵�ƽ
	GPIOB->ODR &= ~(1U<<8);
	//printf("��������ʼ�����");
}

//��������ʼ����
//ͨ��PB8��������ߵ�ƽ10us,���Գ�������trig
void SR04_Start(void){
	GPIOB->ODR |= (1U<<8);
	
	baseTim6Delay_Us(10);
	
	GPIOB->ODR &= ~(1U<<8);
	//printf("����һ�γ�����");
}


//�����������ĳ�ʼ��
void LightSensor(void){
	//��ʼ��ADC1���������
	adc1_PC2_ch12_init();
}

//�������ĳ�ʼ��
void gasDetection(void){
	//��ʼ��ADC2���������
	adc2_PC0_ch10_init();
}

//�����Ͷ���ͬADC��ʼ��
void light_gas_detection_init(void){
	//��ʼ��ADC1���������
	adc1_PC2PC0_ch12ch10ch16_init();
}

//��ͨ��ʹ���жϷ�ʽ��ȡ����
void light_gas_temp_interrupt_init(void){
	adc1_interrupt_init();
}