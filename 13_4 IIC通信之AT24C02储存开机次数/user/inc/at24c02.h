#ifndef AT24C02_H
#define AT24C02_H
#include "stm32f4xx.h"                  // Device header


extern char at24c02_rec_str[];

// AT24C02ҳ��С����
#define PAGE_SIZE 8
#define EEPROM_SIZE 256


//��д��Ӧ��������ַ
#define AT24C02_ADDR_W 0xa0
#define AT24C02_ADDR_R 0xa1

//���ش���״̬
#define AT24C02_NO_ERR 0   //Ӧ��  �޴���
#define	AT24C02_ERR1   1   //��Ӧ��
#define	AT24C02_ERR2   2   //��Ӧ��
#define	AT24C02_ERR3   3   //��Ӧ��
#define	AT24C02_OVER   4   //���ҳ�߽�



void at24c02_init();
u8 at24c02_write_byte(u8 inner_addr,u8 data);
u8 at24c02_read_byte(u8 inner_addr,u8 *data);
u8 at24c02_page_write(u8 inner_addr,const char *address,u8 length);
u8 at24c02_page_read(u8 inner_addr,char *data,u8 length);
void at24c02_skip_page_write(u8 inner_addr,const char *address,u8 length);

#endif