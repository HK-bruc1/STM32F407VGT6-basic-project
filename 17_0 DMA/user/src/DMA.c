#include "DMA.h"
#include "stm32f4xx.h"                  // Device header

void dmaToUsart1(u8* data,u32 data_size)
{
	//DMA时钟使能
	RCC->AHB1ENR |= (1<<22);
	//数据流关闭
	DMA2_Stream7->CR &= ~(1<<0);
	//等待关闭完成
	while(DMA2_Stream7->CR & (1<<0));
	//DMA_SxCR 
	DMA2_Stream7->CR &= ~(7<<25);
	DMA2_Stream7->CR |= (4<<25);     //通道4
	DMA2_Stream7->CR &= ~(3<<16);    //低优先级
	DMA2_Stream7->CR &= ~(1<<9);     //外设递增模式固定
	DMA2_Stream7->CR |= (1<<10);     //存储器递增
	DMA2_Stream7->CR &= ~(3<<11);    //外设大小1byte
	DMA2_Stream7->CR &= ~(3<<13);    //存储器偏移1byte
	DMA2_Stream7->CR &= ~(3<<6);
	DMA2_Stream7->CR |= (1<<6);      //存储器到外设
	DMA2_Stream7->CR &= ~(1<<5);     //DMA流控
	
	//DMA_SxNDTR
	DMA2_Stream7->NDTR = data_size;
	//DMA_SxPAR
	DMA2_Stream7->PAR = (u32)&USART1->DR;
	//DMA_SxM0AR
	DMA2_Stream7->M0AR = (u32)data;
	//外设DMA使能
	//CR3
	USART1->CR3 |= (1<<7);     //使能发送DMA
	
	//开启数据流
	DMA2_Stream7->CR |= (1<<0);

}