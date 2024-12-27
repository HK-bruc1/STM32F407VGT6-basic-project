#ifndef ADC_H
#define ADC_H
#include "stm32f4xx.h"                  // Device header

//都在头文件中定义了，包含就可见，如果只是声明在这的话，要加extern（全局变量）
//用于传递数据
typedef struct {
	//光敏传感器数据
	u16 lightData;
	//气体检测数据
	u16 gasData;
	//温度数据
	float temp;
}ADC1_t;


void adc1_PC2_ch12_init(void);
u16 adc1_PC2_ch12_get(void);
void adc2_PC0_ch10_init(void);
u16 adc2_PC0_ch10_get(void);
void adc1_PC2PC0_ch12ch10ch16_init(void);
ADC1_t adc1_PC2PC0_ch12ch10ch16_get(void);
void adc1_interrupt_init(void);

#endif