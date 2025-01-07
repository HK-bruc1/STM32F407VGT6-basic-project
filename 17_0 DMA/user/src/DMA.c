#include "DMA.h"
#include "stm32f4xx.h"                  // Device header

void dmaToUsart1(u8* data,u32 data_size)
{
	//DMAʱ��ʹ��
	RCC->AHB1ENR |= (1<<22);
	//�������ر�
	DMA2_Stream7->CR &= ~(1<<0);
	//�ȴ��ر����
	while(DMA2_Stream7->CR & (1<<0));
	//DMA_SxCR 
	DMA2_Stream7->CR &= ~(7<<25);
	DMA2_Stream7->CR |= (4<<25);     //ͨ��4
	DMA2_Stream7->CR &= ~(3<<16);    //�����ȼ�
	DMA2_Stream7->CR &= ~(1<<9);     //�������ģʽ�̶�
	DMA2_Stream7->CR |= (1<<10);     //�洢������
	DMA2_Stream7->CR &= ~(3<<11);    //�����С1byte
	DMA2_Stream7->CR &= ~(3<<13);    //�洢��ƫ��1byte
	DMA2_Stream7->CR &= ~(3<<6);
	DMA2_Stream7->CR |= (1<<6);      //�洢��������
	DMA2_Stream7->CR &= ~(1<<5);     //DMA����
	
	//DMA_SxNDTR
	DMA2_Stream7->NDTR = data_size;
	//DMA_SxPAR
	DMA2_Stream7->PAR = (u32)&USART1->DR;
	//DMA_SxM0AR
	DMA2_Stream7->M0AR = (u32)data;
	//����DMAʹ��
	//CR3
	USART1->CR3 |= (1<<7);     //ʹ�ܷ���DMA
	
	//����������
	DMA2_Stream7->CR |= (1<<0);

}