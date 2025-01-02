#include "stm32f4xx.h"                  // Device header
#include "LED.h"
#include "wang_time.h"
#include "BEEP.h"
#include "key.h"
#include "usart.h"
#include "nvic.h"
#include "selfprintf.h"
#include "exti.h"
#include "systick.h"
#include "baseTIM.h"
#include "GeneralTim.h"
#include "redThink.h"
#include "functions.h"
#include "RNG.h"
#include "event_handler.h"
#include "at24c02.h"
#include "string.h"
#include "w25q64.h"
#include "iwdg.h"
#include "rtc.h"


int main(){
	//�������ȼ����飨ֻ��һ����������������
	NVIC_SetPriorityGrouping(5); // ��ռ���ȼ� 2 λ����Ӧ���ȼ� 2 λ
	
	
	//���г�ʼ��
	
	//��������ʼ��
	beepInit();
	
	//����1��ʼ���Ա��ӡ��Ϣ���Լ�������Ϣ
	usart1Init(115200);
	//RTC��ʼ��
	rtc_init();
	
	//��ʱ��7��ʼ��
	baseTim7Interrupt_Init(1);
	
	
	
	//�ȿ����
	BEEP_ON;
	baseTim6Delay_Ms(50);
	BEEP_OFF;
	
	
	
	while(1){
		//ÿ��һ���ӡһ��������
		baseTim7DelayPlus_Ms(1000);
		RTC_T current_time =  rtc_get();
		// ��ʽ����������ӿɶ���
        printf("Current Time: %04d-%02d-%02d Week:%d %02d:%02d:%02d\n", 
               current_time.years, 
               current_time.mon, 
               current_time.date, 
               current_time.week,
               current_time.h, 
               current_time.m, 
               current_time.s);
	}
}