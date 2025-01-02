#include "stm32f4xx.h"                  // Device header
#include "rtc.h"
#include "selfprintf.h"

//rtc��ʼ��
void rtc_init(){
	// ʹ��PWRʱ��
    RCC->APB1ENR |= (1U<<28);  // ʹ��PWRʱ��
	//RCC_BDCR �Ĵ����� RTC �Ĵ������������ݼĴ������Լ� PWR_CSR �Ĵ����� BRE λ���ܵ�д���ʱ���
	//��ΪҪ�޸�RCC->BDCR��ֵ������Ҫ��ǰ�棡
		PWR->CR |= (1U<<8);

	//ѡ��rtcʱ����Դ-��������ƼĴ��� (RCC_BDCR)
		//�����ⲿ����ʱ��
		RCC->BDCR |= (1U<<0);
		//�ȴ�ʱ�Ӿ���
		while(!(RCC->BDCR & (1U<<1)));
		//ѡ��RTCʱ��Դ
		RCC->BDCR &= ~(3U<<8);
		RCC->BDCR |= (1U<<8);
		//ʹ��RTC
		RCC->BDCR |= (1U<<15);
	
	//���TRC��������д����
	//PWR->CR |= (1U<<8);
	//����Ĵ�����д����
		RTC->WPR = 0xCA;
		RTC->WPR = 0x53;
	
	//������ʼ���������ں�ʱ��(��ʼ������)
		//������ʼ��ģʽ
		RTC->ISR |= (1U<<7);
		//�ȴ���ʼ�����
		while(!(RTC->ISR & (1U<<6)));
		//����ʱ���ʽ
		RTC->CR &= ~(1U<<6);
		//����Ӱ�ӼĴ���
		RTC->CR &= ~(1U<<5);
		//Ԥ��Ƶ����Ĭ�Ͼ��ǣ�1HZ
		//����ʱ�� ��Ϊ "|=" �����Ҳ����ı�ԭֵ
		//16~21��λ��Сʱ��BCD��ʽ  0x12
		RTC->TR |= (0x12<<16);
		//8~14��λ�����ӵ�BCD��ʽ 0x12
		RTC->TR |= (0x12<<8);
		//0~6��λ�����BCD��ʽ 0x12
		RTC->TR |= (0x12<<0);
		
		//��������
		RTC->DR = 0;
		//16~23��λ����ݵ�BCD��ʽ 0x12
		RTC->DR |= (0x12<<16);
		//13~15 ���ڼ��ĸ�λ 0X7
		RTC->DR |= (0x7<<13);
		//8~12��λ���·ݵ�BCD��ʽ 0x12
		RTC->DR |= (0x12<<8);
		//0~5��λ���յ�BCD��ʽ 0x12
		RTC->DR |= (0x12<<0);
		
		//�˳���ʼ��ģʽ
		RTC->ISR &= ~(1U<<7);
	
	//����Ĵ�����д����
	RTC->WPR = 0xFF;
}


//���ݷ���˼��
//��λ��ʮ����תBCD��
u8 in_dec_out_bcd(u8 dec){
	return ((dec/10)<<4) | (dec%10); 
}


//BCD��ת��λʮ������
u8 in_bcd_out_dec(u8 bcd){
	return (bcd >> 4)*10 + (bcd & 0x0f);
}


//������һ��ʱ������ڵ����ñ������ó�ʼ�����ѳ�ʼ��һ�����ó���������װ��
void rtc_set(RTC_T data){
	//������ݵ���ʱ����
	u32 temp_d = 0;
	u32 temp_t = 0;
	//����Ĵ���д������������д�����Ѿ��ڳ�ʼ���п�����û�йرգ�
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
	//�����������������ʼ��ģʽ
	RTC->ISR |= (1U<<7);
	//�ȴ���ʼ�����
	while(!(RTC->ISR & (1U<<6)));
	//��ʮ��������ת��ΪBCD��
	temp_d = (in_dec_out_bcd(data.years-2000)<<16)|
					 (in_dec_out_bcd(data.week)<<13)|
					 (in_dec_out_bcd(data.mon)<<8)|
					 (in_dec_out_bcd(data.date));
	
	temp_t = (in_dec_out_bcd(data.h)<<16)|
					 (in_dec_out_bcd(data.m)<<8)|
					 (in_dec_out_bcd(data.s));
	
	//ͳһ����ʱ�������
	RTC->DR = temp_d;
	RTC->TR = temp_t;
	
	//�˳���ʼ��ģʽ
	RTC->ISR &= ~(1U<<7);
	
	//����Ĵ�����д����
	RTC->WPR = 0xFF;
}


//��ȡ����
RTC_T rtc_get(void){
	//������ݵ���ʱ����
	u32 temp_d = 0;
	u32 temp_t = 0;
	RTC_T data = {0};
	
	//�����������ζ�ȡ����ʱ��Ĵ��������ڼĴ���
	//Ϊ��ȷ�������µ�ʱ����Ҫ�Ȱѱ�־λ����
	RTC->ISR &= ~(1U<<5);
	//�ȴ����ݼ��ص�Ӱ�ӼĴ���
	while(!(RTC->ISR & (1U<<5)));
	//��ȡ����
	temp_d = RTC->DR;
	//�ֶ������ٵȴ�һ��
	RTC->ISR &= ~(1U<<5);
	while(!(RTC->ISR &= ~(1U<<5)));
	//��ȡʱ��
	temp_t = RTC->TR;
	
	//������ݿ�ʼ��ֵ�ṹ��
	data.years = in_bcd_out_dec((temp_d >> 16) & 0xff) + 2000;
	data.week = in_bcd_out_dec((temp_d >> 13) & 0x7);
	data.mon = in_bcd_out_dec((temp_d >> 8) & 0x1f);
	data.date = in_bcd_out_dec((temp_d >> 0) & 0x3f);
	
	data.h = in_bcd_out_dec((temp_t >> 16) & 0x3f);
	data.m = in_bcd_out_dec((temp_t >> 8) & 0x7f);
	data.s = in_bcd_out_dec((temp_t >> 0) & 0x7f);
	
	return data;
}