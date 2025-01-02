#include "GeneralTim.h"
#include "stm32f4xx.h"                  // Device header
#include "key.h"
#include "wang_time.h"
#include "selfprintf.h"
#include "redThink.h"
#include "nvic.h"


void tim3_led3_Init(void){
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
	//printf("tim3_led3��ʼ���ɹ�\r\n");
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

//���ƶ���ĳ�ʼ����ʱ��14
void tim14_ServoInit(void){
	/*IO����������*/
	//�˿�ʱ��ʹ�� PA7
	RCC->AHB1ENR |= (1U<<0);
	//�˿�ģʽ����-------����ģʽ
	GPIOA->MODER &= ~(3U<<14);
	GPIOA->MODER |= (1U<<15);
	//�������
	GPIOA->OTYPER &= ~(1U<<7);
	//����ٶ�
	GPIOA->OSPEEDR &= ~(3U<<14);
	GPIOA->OSPEEDR |= (1U<<15);
	//������
	GPIOA->PUPDR &= ~(3U<<14);
	//���ù��ܼĴ���
	GPIOA->AFR[0] &= ~(15U<<28);
	GPIOA->AFR[0] |= (9U<<28);
	
	/*ͨ�ö�ʱ������������*/
	//��ʱ��ʱ��ʹ��
	RCC->APB1ENR |= (1U<<8);
	//CR1
	//��Ӱ�ӼĴ���
	TIM14->CR1 |= (1U<<7);
	//���ض���
	TIM14->CR1 &= ~(3U<<5);
	TIM14->CR1 &= ~(1U<<4);
	TIM14->CR1 &= ~(1U<<3);
	TIM14->CR1 &= ~(1U<<1);
	
	//SMCR
	//ʱ��Դ�����ڲ�ʱ��
	TIM14->SMCR &= ~(7U<<0);
	
	//CCMRx 
	TIM14->CCMR1 &= ~(3U<<0);
	TIM14->CCMR1 |= (1U<<3);
	TIM14->CCMR1 &= ~(7U<<4);
	TIM14->CCMR1 |= (3U<<5);
	
	//CCER
	TIM14->CCER &= ~(1U<<3);
	//����ߵ�ƽΪ��Ч���������øߵ�ƽ
	TIM14->CCER &= ~(1U<<1);
	//���ö�ʱ��3��Ԥ��Ƶ����PSC��Ϊ1679����Ϊ��Ƶϵ��=PSC+1��
	TIM14->PSC = 1679;
	//ARR
	TIM14->ARR = 999;
	//CCRx
	//��ʼ0��
	TIM14->CCR1 = 25;
	//EGR �ֶ����ɸ����¼���װ��ֵ
	TIM14->EGR |= (1U<<0); 
	
	//tim14ͨ��1���ʹ��
	TIM14->CCER |= (1U<<0);
	//������ʹ��
	TIM14->CR1 |= (1U<<0);
	//printf("�����ʼ�����");
}

//�������ƶ���ĽǶ�,��ʼCCR1ֵΪ25����Ӧ0�ȣ���ʱ��14
void tim14KeyServo(void){
	//��ס��һ�ε�ֵ
	static u16 currentAngle;
	u8 keyVal = 0;

	keyVal = key_scan();

	switch(keyVal)
	{
			case 1: // �����������õ�180�ȵİ���
					printf("���õ�180��\r\n");
					currentAngle = 125;
					TIM14->CCR1 = currentAngle;
					break;
			case 2: // �����������ӽǶȵİ���
					printf("���ӽǶ�\r\n");
					if(currentAngle < 125) // ȷ�����������ֵ��180�ȣ�
					{
							currentAngle += 5; // ÿ������5����ζ�ŽǶ�����15��
					}
					TIM14->CCR1 = currentAngle; // ����PWMռ�ձ�
					break;
			case 3: // �����������õ�0�ȵİ���
					printf("���õ�0��\r\n");
					currentAngle = 25;
					TIM14->CCR1 = currentAngle;
					break;
			case 4: // �������Ǽ��ٽǶȵİ���
					printf("���ٽǶ�\r\n");
					if(currentAngle > 25) // ȷ����������Сֵ��0�ȣ�
					{
							currentAngle -= 5; // ÿ�μ���5����ζ�ŽǶȼ���15��
					}
					TIM14->CCR1 = currentAngle; // ����PWMռ�ձ�
					break;
			default:
					// �޲���
					break;
	}
}

//��ʱ��13�����ʼ��
void tim13_MotorInit(void){
	/*IO����������*/
	//�˿�ʱ��ʹ�� PA6
	RCC->AHB1ENR |= (1U<<0);
	//�˿�ģʽ����-------����ģʽ
	GPIOA->MODER &= ~(3U<<12);
	GPIOA->MODER |= (1U<<13);
	//�������
	GPIOA->OTYPER &= ~(1U<<6);
	//����ٶ�
	GPIOA->OSPEEDR &= ~(3U<<12);
	GPIOA->OSPEEDR |= (1U<<13);
	//������
	GPIOA->PUPDR &= ~(3U<<12);
	//���ù��ܼĴ���
	GPIOA->AFR[0] &= ~(15U<<24);
	GPIOA->AFR[0] |= (9U<<24);
	
	/*ͨ�ö�ʱ������������*/
	//��ʱ��ʱ��ʹ��
	RCC->APB1ENR |= (1U<<7);
	//CR1
	//��Ӱ�ӼĴ���
	TIM13->CR1 |= (1U<<7);
	//���ض���
	TIM13->CR1 &= ~(3U<<5);
	TIM13->CR1 &= ~(1U<<4);
	TIM13->CR1 &= ~(1U<<3);
	TIM13->CR1 &= ~(1U<<1);
	
	//SMCR
	//ʱ��Դ�����ڲ�ʱ��
	TIM13->SMCR &= ~(7U<<0);
	
	//CCMRx 
	TIM13->CCMR1 &= ~(3U<<0);
	TIM13->CCMR1 |= (1U<<3);
	TIM13->CCMR1 &= ~(7U<<4);
	TIM13->CCMR1 |= (3U<<5);
	
	//CCER
	TIM13->CCER &= ~(1U<<3);
	//����ߵ�ƽΪ��Ч���������øߵ�ƽ
	TIM13->CCER &= ~(1U<<1);
	//���ö�ʱ��13��Ԥ��Ƶ����PSC��Ϊ839����Ϊ��Ƶϵ��=PSC+1��
	//��Ƶ�����Ƶ��Ϊ100KHZ 100,000��/s  100��/ms 
	TIM13->PSC = 840-1;
	//ARR
	TIM13->ARR = 10-1;
	//CCRx
	//�趨CCRxΪ0ʱ��ռ�ձ�Ϊ0%��
	TIM13->CCR1 = 0;
	//EGR �ֶ����ɸ����¼���װ��ֵ
	TIM13->EGR |= (1U<<0); 
	
	//tim13ͨ��1���ʹ��
	TIM13->CCER |= (1U<<0);
	//������ʹ��
	TIM13->CR1 |= (1U<<0);
	//printf("�����ʼ�����");
}
//�������Ƶ��ת��
void tim13KeyMotor(void){
    // ��̬�������ڼ�ס��һ�ε�PWMֵ�������ת�٣�
    static u16 currentSpeed;
		static u8 statuFlag = 0;
    u8 keyVal = 0;

    keyVal = key_scan();

		switch(keyVal)
    {
        case 1: // �������
            printf("�������\r\n");
						if(statuFlag==0){
							currentSpeed = 1; // ����һ������ת�٣��������10%ռ�ձ�Ϊ����ת��
							TIM13->CCR1 = currentSpeed;
						}else {
							TIM13->CCR1 = currentSpeed;
						}
            break;
        case 2: // ����ת��
            printf("����ת��\r\n");
            if(currentSpeed < 10) 
            {
                currentSpeed += 1; // ÿ������10%ռ�ձȣ���1
            }
            TIM13->CCR1 = currentSpeed; 
            break;
        case 3: // �رյ��
            printf("�رյ��\r\n");
            TIM13->CCR1 = 0;
						statuFlag = 1;
            break;
        case 4: // ����ת��
            printf("����ת��\r\n");
            if(currentSpeed > 1) 
            {
                currentSpeed -= 1; // ÿ�μ���10%ռ�ձȣ���10
            }
            TIM13->CCR1 = currentSpeed; 
            break;
        default:
            break;
    }
}

//�����Ӧ��������͹ر�
void tim13RedThinkMotor(void){
	//��ȡ�����Ӧ����
	if(IR_statu == IR_OK){
		//�򿪵���ĸ���ģʽ����5s��ر�
		TIM13->CCR1 = 1;
		baseTim6DelayPlus_Ms(5000);
		TIM13->CCR1 = 0;
	}
}

//RGBģ��TIM5��ʼ��
void tim5_RGBInit(void){
	/*IO����������*/
	//�˿�ʱ��ʹ�� PA1,2,3
	RCC->AHB1ENR |= (1U<<0);
	//�˿�ģʽ����-------����ģʽ
	GPIOA->MODER &= ~((3U<<2) | (3U<<4) | (3U<<6));
	GPIOA->MODER |= ((1U<<3) | (1U<<5) | (1U<<7));
	//�������
	GPIOA->OTYPER &= ~((1U<<1) | (1U<<2) | (1U<<3));
	//����ٶ�
	GPIOA->OSPEEDR &= ~((3U<<2) | (3U<<4) | (3U<<6));
	GPIOA->OSPEEDR |= ((1U<<3) | (1U<<5) | (1U<<7));
	//������
	GPIOA->PUPDR &= ~((1U<<1) | (1U<<2) | (1U<<3));
	//���ù��ܼĴ���
	GPIOA->AFR[0] &= ~((15U<<4) | (15U<<8) | (15U<<12));
	GPIOA->AFR[0] |= ((2U<<4) | (2U<<8) | (2U<<12));
	
	/*ͨ�ö�ʱ������������*/ //ͨ��2��3��4
	//��ʱ��ʱ��ʹ��
	RCC->APB1ENR |= (1U<<3);
	//CR1
	//��Ӱ�ӼĴ���
	TIM5->CR1 |= (1U<<7);
	//���ض���
	TIM5->CR1 &= ~(3U<<5);
	TIM5->CR1 &= ~(1U<<4);
	TIM5->CR1 &= ~(1U<<3);
	TIM5->CR1 &= ~(1U<<1);
	
	//SMCR
	//ʱ��Դ�����ڲ�ʱ��
	TIM5->SMCR &= ~(7U<<0);
	
	//CCMRx ������ͨ��
	TIM5->CCMR1 &= ~(3U<<8);
	TIM5->CCMR1 |= (1U<<11);
	TIM5->CCMR1 &= ~(7U<<12);
	TIM5->CCMR1 |= (3U<<13);
	
	TIM5->CCMR2 &= ~((3U<<0) | (3U<<8));
	TIM5->CCMR2 |= ((1U<<3) | (1U<<11));
	TIM5->CCMR2 &= ~((7U<<4) | (7U<<12));
	TIM5->CCMR2 |= ((3U<<5) | (3U<<13));
	
	//CCER
	//rgb�͵�ƽΪ��Ч���������õ͵�ƽ
	TIM5->CCER |= (1U<<5);
	TIM5->CCER |= (1U<<7);
	TIM5->CCER |= (1U<<9);
	TIM5->CCER |= (1U<<11);
	TIM5->CCER |= (1U<<13);
	TIM5->CCER |= (1U<<15);
	
	//���ö�ʱ��5��Ԥ��Ƶ����PSC��Ϊ83����Ϊ��Ƶϵ��=PSC+1��
	//1 kHz��PWMƵ��
	TIM5->PSC = 83;
	//ARR
	TIM5->ARR = 999;
	//CCRx
	//��ʼռ�ձȸ�50%
	//PA2 G CCR3    PA3 B CCR4     PA1 R CCR2 
	TIM5->CCR2 = 0;
	TIM5->CCR3 = 0;
	TIM5->CCR4 = 0;
	//EGR �ֶ����ɸ����¼���װ��ֵ
	TIM5->EGR |= (1U<<0); 
	
	//tim14ͨ��2��3��4���ʹ��
	TIM5->CCER |= (1U<<4 | 1U<<8 | 1U<<12);
	//������ʹ��
	TIM5->CR1 |= (1U<<0);
	//printf("RGBģ���ʼ�����");
}




//����1�����벶��
void tim5_KeyCapture(void){
	/*IO����������*/
	//�˿�ʱ��ʹ�� PA0
	RCC->AHB1ENR |= (1U<<0);
	//�˿�ģʽ����-------����ģʽ
	GPIOA->MODER &= ~(3U<<0);
	GPIOA->MODER |= (1U<<1);
	//�������
	GPIOA->OTYPER &= ~(1U<<0);
	//����ٶ�
	GPIOA->OSPEEDR &= ~(3U<<0);
	GPIOA->OSPEEDR |= (1U<<1);
	//������
	GPIOA->PUPDR &= ~(1U<<0);
	//���ù��ܼĴ���
	GPIOA->AFR[0] &= ~(15U<<0);
	GPIOA->AFR[0] |= (2U<<0);
	
	/*ͨ�ö�ʱ������������*/ //ͨ��1
	//��ʱ��ʱ��ʹ��
	RCC->APB1ENR |= (1U<<3);
	//CR1
	//��Ӱ�ӼĴ���
	TIM5->CR1 |= (1U<<7);
	//�˲�������ʱ�Ӳ���Ƶ
	TIM5->CR1 &= ~(3U<<8);
	//���ض���
	TIM5->CR1 &= ~(3U<<5);
	TIM5->CR1 &= ~(1U<<4);
	TIM5->CR1 &= ~(1U<<3);
	TIM5->CR1 &= ~(1U<<2);
	TIM5->CR1 &= ~(1U<<1);
	
	
	//SMCR
	//ʱ��Դ�����ڲ�ʱ��
	TIM5->SMCR &= ~(7U<<0);
	
	
	//CCMRx 
	TIM5->CCMR1 &= ~(3U<<0);
	TIM5->CCMR1 |= (1U<<0);
	TIM5->CCMR1 &= ~(3U<<2);
	TIM5->CCMR1 &= ~(15U<<4);
	TIM5->CCMR1 |= (15U<<4);
	
	
	
	//CCER �����������ش���
	TIM5->CCER &= ~(1U<<1);
	TIM5->CCER &= ~(1U<<3);
	
	//���ö�ʱ��5��Ԥ��Ƶ����PSC��Ϊ83����Ϊ��Ƶϵ��=PSC+1��
	//����Ƶ��Ϊ84000000/84 = 1000KHZ  1000��/ms  1��/us
	TIM5->PSC = 83;
	//ARR
	TIM5->ARR = 65535;

	//EGR �ֶ����ɸ����¼���װ��ֵ
	TIM5->EGR |= (1U<<0); 
	
	//tim5ͨ��1����ʹ��
	TIM5->CCER |= (1U<<0);
	//DIER�ź��� �ж�ʹ��
	TIM5->DIER |= ((1U<<0) | (1U<<1));
	//������ʹ��
	TIM5->CR1 |= (1U<<0);
	
	// 1. �������ȼ�����Ϊ���飨ֻ��һ����������������
    //NVIC_SetPriorityGrouping(5); // ��ռ���ȼ� 2 λ����Ӧ���ȼ� 2 λ

    // 2. �������ȼ�����ֵ
    u32 pri = NVIC_EncodePriority(5, 1, 1); // ����ֵ5����ռ���ȼ� 1����Ӧ���ȼ� 2

    // 3. ���õ�������ж�Դ TIM5_IRQn �����ȼ�
    NVIC_SetPriority(TIM5_IRQn, pri);

    // 4. ���� TIM5_IRQn �ж�
    NVIC_EnableIRQ(TIM5_IRQn);
		printf("���벶���ʼ�����");
}


//��������ECHO���첶�񣨶�ʱ��4�ĳ�ʼ����
void tim4_SRECHO_CaptureInit(void){
	/*IO����������*/
	//�˿�ʱ��ʹ�� PB6
	RCC->AHB1ENR |= (1U<<1);
	//�˿�ģʽ����-------����ģʽ
	GPIOB->MODER &= ~(3U<<12);
	GPIOB->MODER |= (1U<<13);
	//������
	GPIOB->PUPDR &= ~(1U<<12);
	//���ù��ܼĴ���
	GPIOB->AFR[0] &= ~(15U<<24);
	GPIOB->AFR[0] |= (2U<<24);
	
	/*ͨ�ö�ʱ������������*/ //ͨ��1
	//��ʱ��ʱ��ʹ��
	RCC->APB1ENR |= (1U<<2);
	//CR1
	//��Ӱ�ӼĴ���
	TIM4->CR1 |= (1U<<7);
	//�˲�������ʱ�Ӳ���Ƶ
	TIM4->CR1 &= ~(3U<<8);
	//���ض���
	TIM4->CR1 &= ~(3U<<5);
	TIM4->CR1 &= ~(1U<<4);
	TIM4->CR1 &= ~(1U<<3);
	TIM4->CR1 &= ~(1U<<2);
	TIM4->CR1 &= ~(1U<<1);
	
	
	//SMCR
	//ʱ��Դ�����ڲ�ʱ��
	TIM4->SMCR &= ~(7U<<0);
	
	
	//CCMRx 
	TIM4->CCMR1 &= ~(3U<<0);
	TIM4->CCMR1 |= (1U<<0);
	TIM4->CCMR1 &= ~(3U<<2);
	TIM4->CCMR1 &= ~(15U<<4);
	TIM4->CCMR1 |= (15U<<4);
	
	
	
	//CCER �����������ش���
	TIM4->CCER &= ~(1U<<1);
	TIM4->CCER &= ~(1U<<3);
	
	//���ö�ʱ��4��Ԥ��Ƶ����PSC��Ϊ83����Ϊ��Ƶϵ��=PSC+1��
	//����Ƶ��Ϊ84000000/84 = 1000KHZ  1000��/ms  1��/us
	TIM4->PSC = 83;
	//ARR
	TIM4->ARR = 65535;

	//EGR �ֶ����ɸ����¼���װ��ֵ
	TIM4->EGR |= (1U<<0); 
	
	//tim4ͨ��1����ʹ��
	TIM4->CCER |= (1U<<0);
	//DIER�ź��� �ж�ʹ��
	TIM4->DIER |= ((1U<<0) | (1U<<1));
	//������ʹ��
	TIM4->CR1 |= (1U<<0);
	
	// 1. �������ȼ�����Ϊ���飨ֻ��һ����������������
    //NVIC_SetPriorityGrouping(5); // ��ռ���ȼ� 2 λ����Ӧ���ȼ� 2 λ

    // 2. �������ȼ�����ֵ
    u32 pri = NVIC_EncodePriority(5, 0, 0); // ����ֵ5����ռ���ȼ� 1����Ӧ���ȼ� 2

    // 3. ���õ�������ж�Դ TIM4_IRQn �����ȼ�
    NVIC_SetPriority(TIM4_IRQn, pri);

    // 4. ���� TIM4_IRQn �ж�
    NVIC_EnableIRQ(TIM4_IRQn);
		//printf("����SR04��������벶���ʼ�����");
}


