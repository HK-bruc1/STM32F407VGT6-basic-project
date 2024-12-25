#include "nvic.h"
#include "stm32f4xx.h"                  // Device header
#include "selfprintf.h"
#include "systick.h"
#include "wang_time.h"
#include "LED.h"


//����һ���ṹ��������Զ���ʼ��
USART1BAL_t usart1_val;

//�ѵ���һ����ʶ��չ��ȫ��,�Ա�ס�������ж�
volatile u8 countFlag;
//ȫ�ּ������жϾ����̶���
volatile u32 count;

//��1ms�ж�һ�Σ����������¼������ж�
volatile u32 sysTickCount[5];

//��1ms�ж�һ�Σ����������¼������ж�
volatile u32 TIM7Count[5];

//��1ms�ж�һ�Σ����������¼������ж�
volatile u32 TIM6Count[5];



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
		//�ӳٺ�����
		TIM6Count[1]--;
		
		if(TIM6Count[0]==3000){
			printf("������ʱ��6��ʱ�жϲ���\r\n");
			TIM6Count[0]=0;
		}
		
	}
	
}


//������ʱ��TIM7���жϷ�����
void TIM7_IRQHandler(void){
	if(TIM7->SR & (1U<<0)){
		//�����־λ,��������ж���Ҫ�����־λ�����Ա���Ҫ��0
		//��ϵͳ�δ�ʱ����һ��
		TIM7->SR &= ~(1U<<0);
		
		
		//����led�����ı�־λ
		static  u8 ledBreathFlag = 1; 
		
		//��ӡ��
		TIM7Count[0]++;
		//LED3��������
		TIM7Count[1]++;
		
		if(TIM7Count[0]==3000){
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
			//ÿһ������ά��3ms
			TIM7Count[1]=0;
		}
	}
}

