#ifndef SPI_H
#define SPI_H
#include "stm32f4xx.h"                  // Device header

#define SPI_SCLK_L  (GPIOA->ODR &= ~(1<<5))
#define SPI_SCLK_H  (GPIOA->ODR |= (1<<5))
#define SPI_MOSI_L  (GPIOA->ODR &= ~(1<<7))
#define SPI_MOSI_H  (GPIOA->ODR |= (1<<7))
#define SPI_MISO     (GPIOA->IDR & (1<<6))
#define SPI_TIMEOUT 10000


void spi1_init(void);
u8 spi1_byte(u8 data);
u8 spi1_controller_TransmitReceive(u8 data);
void spi1_controller_init(void);

#endif