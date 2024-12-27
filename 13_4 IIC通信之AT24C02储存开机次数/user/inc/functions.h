#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "stm32f4xx.h"                  // Device header

//����������鼮�ṹ������AT24C02����ʹ��
typedef struct {
    u8 name[30]; // 15���ַ������15�����֣�30�ֽڣ�GB2312
    u8 writer[30]; // 15���ַ������15�����֣�30�ֽ�
    u8 number[15]; // ����number����ASCII�ַ�
    u32 hot;
    u32 sc;
    float price;
} Book;


extern volatile u8 randomColorFlag;

void PrintBootCount(void);
void WriteBookToEEPROM(Book *book, u16 baseAddr);
void ReadBookFromEEPROM(Book *book, uint16_t baseAddr);
void PrintBook(const char* prefix, const Book* book);
void TestBookStorage(void);

void tim5_Key_RGB(void);
void changeColorRandomly(void);
void SR04_Init(void);
void SR04_Start(void);
void LightSensor(void);
void gasDetection(void);
void light_gas_detection_init(void);
void light_gas_temp_interrupt_init(void);

#endif