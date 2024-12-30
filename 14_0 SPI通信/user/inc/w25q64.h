#ifndef W25Q64_H
#define W25Q64_H
#include "stm32f4xx.h"                  // Device header


// W25Q64 Ö¸Áî
#define W25Q64_WRITE_ENABLE  0x06  // Ð´Ê¹ÄÜ
#define W25Q64_READ_DATA     0x03  // ¶ÁÈ¡Êý¾Ý
#define W25Q64_PAGE_PROGRAM  0x02  // Ò³±à³Ì
#define W25Q64_READ_STATUS   0x05  // ¶ÁÈ¡×´Ì¬¼Ä´æÆ÷
#define W25Q64_WRITE_STATUS  0x01  // Ð´×´Ì¬¼Ä´æÆ÷
#define W25Q64_SECTOR_ERASE  0x20  // ÉÈÇø²Á³ý
#define W25Q64_BLOCK_ERASE   0xD8  // ¿éÇø²Á³ý
#define W25Q64_CHIP_ERASE    0xC7  // Ð¾Æ¬²Á³ý

#define W25Q64_CS_LOW (GPIOB->ODR &= ~(1U<<14))   // Ñ¡ÖÐÆ¬Ñ¡Òý½Å
#define W25Q64_CS_HIGH (GPIOB->ODR |= (1U<<14))   // ÊÍ·ÅÆ¬Ñ¡Òý½Å


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