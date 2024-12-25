#include "event_handler.h"
#include "nvic.h"
#include "adc.h"
#include "selfprintf.h"
#include "BEEP.h"
#include "LED.h"
#include "functions.h"


//����״̬��ɨ����¼��Ĵ�������
void Scanning_Events(void){
	//�����͹���
	u16 LightData;
	u16 GasData;
	u16 brightness;
	//ͬADC1��ͬͨ��ת���Ķ����͹����ṹ��
	ADC1_t val;
	//����й©��־λ
	u8 gasLeak;
	
	//ÿ100ms��ȡ�¶ȣ����壬�������ݲ�������Ӧ���¼�
	if(TIM7Count[7]>=100){
		TIM7Count[7] = 0;
		val = adc1_PC2PC0_ch12ch10ch16_get();
		//���ص������Ѿ�ת����0~100֮����
		// ��0-100�Ĺ��հٷֱ�ӳ�䵽0-1000��LED���ȷ�Χ
		brightness = (val.lightData * 10); // ���ٷֱ�ת��Ϊ0-1000��Χ
		// ����LED����
		TIM3->CCR1 = brightness;
		
		// ��������
		//Ũ�ȳ���20%���ſ��������Ũ��Խ��Խ�죬�ȳ�ʼ��tim13_MotorInit
		if(val.gasData > 20){
				//������й©��־��1
				gasLeak = 1;
				// ��20-100��Ũ�ȷ�Χӳ�䵽0-10��ת�ٷ�Χ
				// Ũ��ÿ����8���ٶ�����1
				u8 speed = (val.gasData - 20) / 8; 
				
				// ���õ���ٶ�
				TIM13->CCR1 = speed;
				
				printf("����Ũ�ȣ�%d%%  ����ٶȵ�λ��%d\r\n", val.gasData,speed);
		}else{
				//������й©��־��0
				gasLeak = 0;
				//�ѷ��������ˣ���־λ��0���������ȥ���޷��ر�
				//BEEP_OFF;
				LED1_OFF;
				// Ũ�ȵ���20%ʱֹͣ���
				TIM13->CCR1 = 0;
				printf("����Ũ�ȣ�%d%%  ������ڹر�״̬\r\n",val.gasData);
		}
		
		printf("����ǿ�Ȱٷֱȣ�%d%%  LED����ֵ��%d\r\n", val.lightData, brightness);
		printf("�����ڲ��¶����ݣ�%.2f��C\r\n",val.temp);
		
		//����й©��һЩ�¼�
		//�ж��Ƿ������������
		if(gasLeak==1){
			//��ʼ����������
			SR04_Start();
			//��ʼ������������ÿ����ٴ�,�ȳ�ʼ��tim14_ServoInit
			TIM14->CCR1 = 125;
		}
	}
}