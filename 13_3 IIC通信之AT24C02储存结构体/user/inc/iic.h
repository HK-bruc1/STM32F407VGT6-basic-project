#ifndef IIC_H
#define IIC_H
#include "stm32f4xx.h"                  // Device header

//宏定义和函数声明
#define IIC_SCL_L (GPIOB->ODR &= ~(1U<<6))
#define IIC_SCL_H (GPIOB->ODR |= (1U<<6))

#define IIC_SDA_OUT_L (GPIOB->ODR &= ~(1U<<7))
#define IIC_SDA_OUT_H (GPIOB->ODR |= (1U<<7))

#define IIC_SDA_IN (GPIOB->IDR &(1U<<7))




void iic_io_init(void);
void iic_start(void);
void iic_stop(void);
void iic_send_byte(u8 data);
u8 iic_rec_byte(void);
void iic_send_ack(u8 ack);
u8 iic_get_ack(void);


#endif