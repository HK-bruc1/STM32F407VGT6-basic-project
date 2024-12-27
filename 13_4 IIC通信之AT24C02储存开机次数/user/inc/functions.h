#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "stm32f4xx.h"                  // Device header

//定义和声明书籍结构体用于AT24C02传输使用
typedef struct {
    u8 name[30]; // 15个字符，最多15个汉字，30字节，GB2312
    u8 writer[30]; // 15个字符，最多15个汉字，30字节
    u8 number[15]; // 假设number都是ASCII字符
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