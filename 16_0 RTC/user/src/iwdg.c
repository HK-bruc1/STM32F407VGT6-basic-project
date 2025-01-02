#include "iwdg.h"
#include "stm32f4xx.h"                  // Device header
#include "selfprintf.h"

void iwdg_init(void)
{
    //ʱ��Դ����-��������ʱ��
    RCC->CSR |= (1U<<0);
    //�ȴ�ʱ���ȶ�
    while(!(RCC->CSR & (1U<<1)));
    
    //���д����
    IWDG->KR = 0x5555;
    //���÷�ƵΪ32��Ƶ
    IWDG->PR = 3;     //ֱ�Ӹ�ֵ������
    //��װ��ֵ����Ϊ3000 (��ʱʱ��Լ3��)
    IWDG->RLR = 3000;
    //���ص�������
    IWDG->KR = 0xAAAA;
    //�������Ź�
    IWDG->KR = 0xCCCC;
    
    printf("���Ź��Ѿ�����\r\n");
}


//����ι������
void iwdg_feed(void)
{
    IWDG->KR = 0xAAAA;
}