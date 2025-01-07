#include "nvic.h"
#include "stm32f4xx.h"                  // Device header
#include "selfprintf.h"
#include "systick.h"
#include "wang_time.h"
#include "LED.h"
#include "GeneralTim.h"
#include "BEEP.h"
#include "adc.h"
#include "functions.h"



//����һ���ṹ��������Զ���ʼ��
USART1BAL_t usart1_val;

//�ѵ���һ����ʶ��չ��ȫ��,�Ա�ס�������ж�
volatile u8 countFlag;
//ȫ�ּ������жϾ����̶���
volatile u32 count;

//��1ms�ж�һ�Σ����������¼������ж�
volatile u32 sysTickCount[5];

//��1ms�ж�һ�Σ����������¼������ж�
volatile u32 TIM7Count[15];

//��1ms�ж�һ�Σ����������¼������ж�
volatile u32 TIM6Count[5];

//��һ����־λ��������Ӧ�Ƿ����ϰ�
volatile u8 IR_statu;

//��ʱ�ж���ˮ���ٶȿ���(0~10)
volatile u8 Tim7WaterLed_Speed;
volatile u8 waterLedOpen_Flag;




//����1��������жϴ�����
void USART1_IRQHandler(void) {
    //USART1 �жϴ�����
		//����һ���ֽ�������ɴ����ж�
    if (USART1->SR & (1U<<5)) {
        usart1_val.usart1_buff[usart1_val.length] = USART1->DR; //ͨ���� USART_DR �Ĵ���ִ�ж�����������ж�״̬λ����
				usart1_val.length++;
    }
		
		//���������ֽ����ݺ���·����ʱ�����ж�
		//��λ������������㣨���� USART_SR �Ĵ�����Ȼ����� USART_DR �Ĵ�����
		if(USART1->SR & (1U<<4)){
				//�����־λ
				//USART1->SR;
				USART1->DR;
				//������ѻ���������������һ��'\0'
				usart1_val.usart1_buff[usart1_val.length] = '\0';
				//���û������±�
				usart1_val.length = 0;
				//���Զ����β��0��������
				printf("usart1_buff:\t%s\r\n",usart1_val.usart1_buff);
				//��һ���ַ���������ɵı�ʶ������ȫ�ֱ����������ⲿ�ж�
				usart1_val.usart1StrRec_flag = 1;
		}
}


//EXTI8�жϷ�����
void EXTI9_5_IRQHandler(void){
	//��һ����ʱ��̬�ֲ��������ڼ���
	//static u32 count = 0;
	//��һ����ʶ����������ʹ��
	static u8 extiFlag = 1;
	
	//���õ��ߣ����жϾ�������һ���ж��ź�
	if(EXTI->PR & (1<<8)){
		//�����״̬λ,��1���״̬
		EXTI->PR |= (1<<8);
		//���º��ɿ���������ж�
		delay_ms(15);
		//ֱ�ӷ��ʼĴ�����ö࣬�����ú����ˡ�����
		if((!(GPIOC->IDR & (1<<8))) && extiFlag){
			//�����¼�
			LED1_TURN;
			count++;
			//printf("�������:%d\r\n",count);
			//��ʶλ��ס�����ɿ�֮ǰ�������¼�����ִ�еڶ���
			extiFlag = 0;
			countFlag = 1;
		}else if((GPIOC->IDR & (1<<8)) && extiFlag==0){
			//��ס����һ�Σ�״̬�������������е���������ڽ���
			//15ms�������е�����ٽ�������ʶλ��ס�ˣ�ֱ�ӳ�ȥ��С���ʣ�
			//�ɿ�ʱ�����һ�Σ��ѱ�ʶλ����
			extiFlag = 1;
			//15ms�������е����,Ҳ������������������
		}
	}
}


//�δ�ʱ�жϷ�����(��Ȼ�൱�ڶ����ģ����ǻ���д��һ��)
void SysTick_Handler(void){
	//��յ�ǰֵ�Ĵ���,�����Ӧ״̬λ
	//�����ⲿ֪���Ͳ�������
	SysTick->VAL = 0;
	//��ӡ��
	sysTickCount[0]++;
	//��ʱ�����ã��޷���int���͵����ݣ�����0֮�󣬻ỷ�νṹ��������������δ������Ϊ
	sysTickCount[1]--;
	
	if(sysTickCount[0]==500){
		//�������ݼ���0�ʹ���һ���ж�
		printf("ϵͳ�δ�ʱ�жϲ���\r\n");
		sysTickCount[0]=0;
	}
}


//������ʱ��TIM6���жϷ�����
void TIM6_DAC_IRQHandler(void){
	
	//������ж�ͨ�����ж���ʲô�ж��ź�
	if(TIM6->SR & (1U<<0)){
		//�����־λ,��������ж���Ҫ�����־λ�����Ա���Ҫ��0
		//��ϵͳ�δ�ʱ����һ��
		TIM6->SR &= ~(1U<<0);
		
		//��ӡ��
		TIM6Count[0]++;
		//�ӳٺ�����(at24c02ʹ��������ʽû�취����)
		//TIM6Count[1]--;
		
		
		if(TIM6Count[0]==300){
			printf("������ʱ��6��ʱ�жϲ���\r\n");
			TIM6Count[0]=0;
		}
	}
	
}


//������ʱ��TIM7���жϷ�����
void TIM7_IRQHandler(void){
	//���ֲ�����
	//����led�����ı�־λ
	static  u8 ledBreathFlag = 1; 
	//������ˮ�Ʊ�ſ���
	static u8 ledNum = 1;
	//�����͹���
	u16 LightData;
	u16 GasData;
	u16 brightness;
	//ͬADC1��ͬͨ��ת���Ķ����͹����ṹ��
	ADC1_t val;
	
	if(TIM7->SR & (1U<<0)){
		//�����־λ,��������ж���Ҫ�����־λ�����Ա���Ҫ��0
		//��ϵͳ�δ�ʱ����һ��
		TIM7->SR &= ~(1U<<0);

		//�����¼�
		//���Դ�ӡ��
		//TIM7Count[0]++;
		//LED3��������
		//TIM7Count[1]++;
		//��ˮ�Ƽ���ʹ��
		//TIM7Count[2]++;
		//�����Ӧʹ��
		//TIM7Count[3]++;
		//ʵ�������Գ���������
		//TIM7Count[4]++;
		//���ڹ����������������Լ�� 100ms
		//TIM7Count[5]++;
		//���ڶ������
		//TIM7Count[6]++;
		//ͬʱ���ڹ����Ͷ����Լ��¶�
		//TIM7Count[7]++;
		//��ͨ���жϷ�ʽ�������Կ���ת����
		//TIM7Count[8]++;
		//������ʽ��ʱ��(��������¼���)
		TIM7Count[9]--;
		
		//�����ж��Ƿ����
		if(TIM7Count[0]==300){
			printf("������ʱ��7��ʱ�жϲ���\r\n");
			TIM7Count[0]=0;
		}
		
		//LED3������
		if(TIM7Count[1]==2){
			if(ledBreathFlag==1){
				TIM3->CCR1++;
				if(TIM3->CCR1 >= 1000){
					ledBreathFlag = 0;
				}
			}else if(ledBreathFlag==0){
				TIM3->CCR1--;
				if(TIM3->CCR1 < 1){
					ledBreathFlag = 1;
				}
			}
			//ÿһ������ά��2ms
			TIM7Count[1]=0;
		}
		
		//�ж��Ƿ�����ˮ��
		if(waterLedOpen_Flag){
			//��ʼ������������ˮ��
			TIM7Count[2]++;
		}
		
		// ��ˮ�ƿ���(Ҫ��ʼ��led),�������һ���ٶ��ж�
		u8 delay = 80 - (Tim7WaterLed_Speed * 5);
		if((TIM7Count[2] >= delay) && Tim7WaterLed_Speed<=10 && Tim7WaterLed_Speed>=0){ // speedΪ30ms��죬ÿ�ε�����5msΪ��λ��Ч������
				// �رյ�ǰLED
				ledX_off(ledNum);
				// �ƶ�����һ��LED
				ledNum = (ledNum % 4) + 1;
				// ���µ�LED
				ledX_on(ledNum);
				TIM7Count[2] = 0;
		}
		
		//�����Ӧÿ3ms�ͼ��һ��״̬
		if(TIM7Count[3]==3){
			TIM7Count[3]=0;
			if(GPIOC->IDR & (1<<8)){
				IR_statu = 0;
			}else {
				IR_statu = 1;
			}
		}
		
		//ÿ100ms����������һ��
		if(TIM7Count[4]==3000){
			TIM7Count[4] = 0;
			SR04_Start();
		}
		
		//ÿ100ms����������ת��һ��
		if(TIM7Count[5]==100){
			TIM7Count[5] = 0;
			// ��ȡADCֵ��ת��Ϊ0-100�İٷֱ�
			LightData = adc1_PC2_ch12_get();
			LightData = (4095-LightData)/4095.0*100;
			// ��0-100�Ĺ��հٷֱ�ӳ�䵽0-1000��LED���ȷ�Χ
			brightness = (LightData * 10); // ���ٷֱ�ת��Ϊ0-1000��Χ
			// ����LED����
			TIM3->CCR1 = brightness;
			printf("����ǿ�Ȱٷֱȣ�%d%%  LED����ֵ��%d\r\n", LightData, brightness);
		}
		
		//ÿ100ms�������һ��
		if(TIM7Count[6]==100){
			TIM7Count[6] = 0;
			// ��ȡADCֵ��ת��Ϊ0-100�İٷֱ�
			GasData = adc2_PC0_ch10_get();
			GasData=GasData/4095.0*100;
			//Ũ�ȳ���20%���ſ��������Ũ��Խ��Խ�죬�ȳ�ʼ��tim13_MotorInit
			if(GasData > 20){
					// ��20-100��Ũ�ȷ�Χӳ�䵽0-10��ת�ٷ�Χ
					// Ũ��ÿ����8���ٶ�����1
					u8 speed = (GasData - 20) / 8; 
					
					// ���õ���ٶ�
					TIM13->CCR1 = speed;
					
					printf("����Ũ�ȣ�%d%%  ����ٶȵ�λ��%d\r\n", GasData,speed);
			}
			else{
					// Ũ�ȵ���20%ʱֹͣ���
					TIM13->CCR1 = 0;
					printf("����Ũ�ȣ�%d%%  ������ڹر�״̬\r\n",GasData);
			}
		}
		
		//��ʱ�Կ���ת���� ÿ1�뿪��һ�Σ�һ��ת������ͨ���������ɴ����жϻ�ȡ
		if(TIM7Count[8]==1000){
			TIM7Count[8] = 0;
			//����ת��
			//printf("����ת����\r\n");
			ADC1->CR2 |= (1U<<30);
		}
		
		//����RGB�������ɫ�ĳ���ʱ��
		if(randomColorFlag==1){
			//����RGB�������ɫ�ĳ���ʱ��
			TIM7Count[9]++;
			//��ɫ����500ms���ٻ�һ����ɫ
			if(TIM7Count[9]==500){
				TIM7Count[9] = 0;
				changeColorRandomly();
			}
		}
		
	}
}


//ͨ�ö�ʱ��5���жϷ�����
void  TIM5_IRQHandler(void){
	static u16 time_n = 0;
	static u16 test_1 = 0;
	u16 test_2 = 0;
	u32 test = 0;
	
	//�ж�����һ���ж��ź�
	//�ж��Ǹ����ж�,���������
	if(TIM5->SR & (1U<<0)){
		//�����־λ
		TIM5->SR &= ~(1U<<0);
		time_n++;
	}
	
	//�ж��ǲ����ж�
	if(TIM5->SR & (1U<<1)){
		//�����־λ
		TIM5->SR &= ~(1U<<1);
		//�����¼�
		//�����������,��Ϊ��ʼ��ʱ�����ˣ����Ե�һ�δ����ض������õ��Ǹ������Կ���ͨ���ж�
		if(!(TIM5->CCER & (1U<<1))){
			time_n = 0;//��ʼ��¼��Ч����
			//��ȡ����Ĵ����е�ֵ
			test_1 = TIM5->CCR1;
			//�л�����2
			TIM5->CCER |= (1U<<1);
		}
		//������½��ز���
		else if(TIM5->CCER & (1U<<1)){
			//��¼����Ĵ����ĵڶ���ֵ
			test_2 = TIM5->CCR1;
			//����������
			test = time_n*65535-test_1+test_2;
			printf("�������µ�ʱ�䣺%0.1fms\r\n",test/1000.0);
			//�ٴ��л�����1
			TIM5->CCER &= ~(1U<<1);
		}
	}
}

//ͨ�ö�ʱ��4���жϷ�����
void  TIM4_IRQHandler(void){
	static u16 time_n = 0;
	static u16 test_1 = 0;
	u16 test_2 = 0;
	u32 test = 0;
	
	//�ж�����һ���ж��ź�
	//�ж��Ǹ����ж�,���������
	if(TIM4->SR & (1U<<0)){
		//�����־λ
		TIM4->SR &= ~(1U<<0);
		time_n++;
	}
	
	//�ж��ǲ����ж�
	if(TIM4->SR & (1U<<1)){
		//�����־λ
		TIM4->SR &= ~(1U<<1);
		//�����¼�
		//�����������,��Ϊ��ʼ��ʱ�����ˣ����Ե�һ�δ����ض������õ��Ǹ������Կ���ͨ���ж�
		if(!(TIM4->CCER & (1U<<1))){
			time_n = 0;//��ʼ��¼��Ч����
			//��ȡ����Ĵ����е�ֵ
			test_1 = TIM4->CCR1;
			//�л�����2
			TIM4->CCER |= (1U<<1);
		}
		//������½��ز���
		else if(TIM4->CCER & (1U<<1)){
			//��¼����Ĵ����ĵڶ���ֵ
			test_2 = TIM4->CCR1;
			//���������� ��λ��us
			test = time_n*65535-test_1+test_2;
			float distance_cm = (34*test/1000.0)/2;
			//С��50cm�ͱ���
			if(distance_cm<=50){
				//BEEP_ON;
				LED1_ON;
				printf("�к�����й©���˳�50cm���⣡��ǰ����Ϊ��%.1fcm\r\n",distance_cm);
			}else {
				//BEEP_OFF;
				LED1_OFF;
			}
			//�ٴ��л�����1
			TIM4->CCER &= ~(1U<<1);
		}
	}
}


//ADC�жϷ�����
void ADC_IRQHandler(void){
	//printf("����ADC�ж�\r\n");
	static u8 count = 0;
	static ADC1_t val;
	//�жϾ������Ǹ�ADC������
	if((ADC1->SR & (1U<<1))>>1){
		//printf("����ADC1�ж�\r\n");
		count++;
		//printf("֮ǰ��ֵ%d\r\n",(ADC1->SR & (1U<<1))>>1);
		//��������
		if(count == 1){
			val.lightData = ADC1->DR;
			val.lightData = (4095-val.lightData)/4095.0*100;
			//printf("%d\r\n",(ADC1->SR & (1U<<1))>>1);
		}else if(count == 2){//��������
			val.gasData = ADC1->DR;
			val.gasData = val.gasData/4095.0*100;
		}else if(count == 3){
			count=0;
			val.temp = ADC1->DR;//�¶�
			val.temp = (val.temp*0.805-760)/2.5+25;
			//�жϸ�����
			if(val.lightData>80){
				//led3����������
				TIM3->CCR1 = 1000;
			}else if(val.lightData<70 && val.lightData>40){
				//led3�����ȼ�һ��
				TIM3->CCR1 = 500;
			}else{
				//led3���������
				TIM3->CCR1 = 100;
			}
			
			if(val.gasData>50){
				//��������
				BEEP_ON;
			}else{
				//��������
				BEEP_OFF;
			}
			printf("����ǿ�Ȱٷֱȣ�%d%%\r\n", val.lightData);
			printf("������Ⱦ�̶Ȱٷֱȣ�%d%%\r\n",val.gasData);
			printf("�¶����ݣ�%.2f��C\r\n",val.temp);
		}
	}
}

