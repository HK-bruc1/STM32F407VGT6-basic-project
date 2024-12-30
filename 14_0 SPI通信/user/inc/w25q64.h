#ifndef W25Q64_H
#define W25Q64_H
#include "stm32f4xx.h"                  // Device header


// W25Q64 ָ��
#define W25Q64_WRITE_ENABLE  0x06  // дʹ��
#define W25Q64_READ_DATA     0x03  // ��ȡ����
#define W25Q64_PAGE_PROGRAM  0x02  // ҳ���
#define W25Q64_READ_STATUS   0x05  // ��ȡ״̬�Ĵ���
#define W25Q64_WRITE_STATUS  0x01  // д״̬�Ĵ���
#define W25Q64_SECTOR_ERASE  0x20  // ��������
#define W25Q64_BLOCK_ERASE   0xD8  // ��������
#define W25Q64_CHIP_ERASE    0xC7  // оƬ����

#define W25Q64_CS_LOW (GPIOB->ODR &= ~(1U<<14))   // ѡ��Ƭѡ����
#define W25Q64_CS_HIGH (GPIOB->ODR |= (1U<<14))   // �ͷ�Ƭѡ����


void W25Q64_Init(void);
void W25Q64_WriteEnable(void);
u8 W25Q64_ReadStatus(void);
void W25Q64_WriteStatus(u8 data);
void W25Q64_ReadData(u32 inner_addr, u8* data, u32 length);
void W25Q64_writePage(u32 inner_addr, u8* data, u16 length);
void W25Q64_SectorErase(u32 inner_addr);
void W25Q64_BlockErase(u32 inner_addr);
void W25Q64_ChipErase();
void W25Q64_Test(void);



#endif