#include "spi.h"
#include "stm32f4xx.h"                  // Device header




//spi����IO�˿ڳ�ʼ����ֱ��ʹ��GPIO��ģ�⣬��ͨ��Ƭ��������⽻��,ͨ��ģʽ
/*
SCK-----PA5     SPI1_SCK  ͨ����� ����
MOSI----PA7     SPI1_MOSI ͨ����� ����
CS------PB14    SPI1_NSS  ͨ����� Ƭѡ
MISO----PA6     SPI1_MISO ͨ������ ����
*/
void spi1_init(void){
    //GPIO���ߵ�ʱ��ʹ��
    RCC->AHB1ENR |= (1<<0) | (1<<1);
    //GPIOģʽ����
    GPIOA->MODER &= ~((3U<<10) | (3U<<14));
    GPIOA->MODER |= (1U<<10) | (1U<<14);
    GPIOA->MODER &= ~(3U<<12);
    //GPIO�������
    GPIOA->OTYPER &= ~(1U<<5 | 1U<<7);//�������
    //GPIO����ٶ�����
    GPIOA->OSPEEDR &= ~((3U<<10) | (3U<<14));
    GPIOA->OSPEEDR |= (2U<<10) | (2U<<14);//50MHz
    //GPIO�������
    GPIOA->PUPDR &= ~((3U<<10) | (3U<<12) | (3U<<14));
		
		// ����PB14Ϊ���豸Ƭѡ���ţ�ͨ�����ģʽ��
    GPIOB->MODER &= ~(3U<<28);  // ���PB14ԭ����
    GPIOB->MODER |= (1U<<28);   // ����Ϊͨ�����ģʽ
    GPIOB->OTYPER &= ~(1U<<14); // �������
    GPIOB->OSPEEDR &= ~(3U<<28);// ����ٶ�����
    GPIOB->OSPEEDR |= (1U<<29); // ����Ϊ50MHz
    GPIOB->PUPDR &= ~(3U<<28);//��������
    GPIOB->ODR &= ~(1U<<14);    // ��ʼ������Ƭѡ����

    //ʱ�Ӽ��� �� 0
    GPIOA->ODR &= ~(1U<<5);
}

//����һ���ֽ�
//ʱ�Ӽ��� 0 ��
//ʱ����λ 0 ��  ��һ�������ض����ݣ�ʱ��һ��ʼ�ǵͣ��̶���һ���������������أ��ڶ������������½���
u8 spi1_byte(u8 data){
    u8 i;
    for(i=0;i<8;i++){
        //����һλ����
        SPI_SCLK_L;
        if(data & 0x80)
            SPI_MOSI_H;
        else
            SPI_MOSI_L;
        data <<= 1;
        
        //����һλ����
        SPI_SCLK_H;
        if(SPI_MISO)
            data |= 0x01;
    }
    return data;
}


/*
SCK-----PA5     SPI1_SCK  ������� ����
MOSI----PA7     SPI1_MOSI ������� ����
CS------PB14    SPI1_NSS  ͨ����� Ƭѡ
MISO----PA6     SPI1_MISO �������� ����
*/
//SPI���������õĳ�ʼ��
void spi1_controller_init(void){
    //GPIO����ʱ��ʹ��
    RCC->AHB1ENR |= (1U<<0);//ʹ��GPIOA

    //GPIO����ģʽ����
    GPIOA->MODER &= ~((3U<<10) | (3U<<12) | (3U<<14)); //���ԭ����
    GPIOA->MODER |= (2U<<10) | (2U<<12) | (2U<<14);    //����Ϊ����ģʽ
    //����������� PA5 PA7
    GPIOA->OTYPER &= ~((1U<<5) | (1U<<7)); // �������
    //��������ٶ�
    GPIOA->OSPEEDR &= ~((3U<<10) | (3U<<14));// ����ٶ�����
    GPIOA->OSPEEDR |= ((1U<<11) | (1U<<15)); // ����Ϊ50MHz
    //����������
    GPIOA->PUPDR &= ~((3U<<10) | (3U<<12) | (3u<<14));//��������
    
    //����GPIO�ĸ��ù���ΪSPI AF5
    GPIOA->AFR[0] &= ~((0xF<<20) | (0xF<<24) | (0xF<<28)); 
    GPIOA->AFR[0] |= (5U<<20) | (5U<<24) | (5U<<28);   //����ΪAF5��SPI1��
		
		//����Ƭѡ�����ã�������ֲ
//    // ����PB14ΪƬѡ���ţ�ͨ�����ģʽ��
//    GPIOB->MODER &= ~(3U<<28);  // ���PB14ԭ����
//    GPIOB->MODER |= (1U<<28);   // ����Ϊͨ�����ģʽ
//    GPIOB->OTYPER &= ~(1U<<14); // �������
//    GPIOB->OSPEEDR &= ~(3U<<28);// ����ٶ�����
//    GPIOB->OSPEEDR |= (1U<<29); // ����Ϊ50MHz
//    GPIOB->PUPDR &= ~(3U<<28);//��������
//    // ȷ��CS���ų�ʼ��Ϊ��
//    GPIOB->ODR |= (1U<<14);  // ����Ƭѡ��ͨ��ʱ�����͡�


    //SPI����
    RCC->APB2ENR |= (1<<12); //SPI1ʱ��ʹ��
    //CR1�Ĵ�������
    SPI1->CR1 &= ~(1U<<15); //˫�ߵ���
    SPI1->CR1 &= ~(1U<<11); //8λ����֡
    SPI1->CR1 &= ~(1U<<10); //˫�ߵ����£�ȫ˫��
    SPI1->CR1 |= (1U<<9); //NSS����ѡ���������
    SPI1->CR1 |= (1U<<8); //NSS���ŵ�ֵ��1������SPI������ͨ�ţ�����ͼ��ֵ
    SPI1->CR1 &= ~(1U<<7); //�ȷ���λ
    SPI1->CR1 &= ~(7U<<3); //�����ʿ���ΪFPCLK/2��84/2=42MHz
    SPI1->CR1 |= (1U<<2); //������
    SPI1->CR1 &= ~(1U<<1); //CPOL=0 CPHA=0
    SPI1->CR1 &= ~(1U<<0); //CPOL=0 CPHA=0

    //CR2�Ĵ�������
    SPI1->CR2 &= ~(1U<<4);//SPI Motorola ģʽ

    SPI1->CR1 |= (1U<<6); //ʹ��SPI
}

//SPIȫ˫�����ͺͽ���һ���ֽڣ����г�ʱ���ƣ�
u8 spi1_controller_TransmitReceive(u8 data){
    //u32 timeout = SPI_TIMEOUT;
    //�ȴ����ͻ�������
    while(!(SPI1->SR & (1U<<1))){
        //if(timeout-- == 0) return 0xFF; // ��ʱ����
    }
    SPI1->DR = data; //�������ݣ����������ݺ󣬽��ջ���������8λ������
    
    //�ȴ���һ�ν��ջ������ǿ�
    //timeout = SPI_TIMEOUT;
    while(!(SPI1->SR & (1U<<0))){
         //if(timeout-- == 0) return 0xFF; // ��ʱ����
    }
    return SPI1->DR; //���ؽ��յ�������
}
