#include "adc.h"
#include "stm32f4xx.h"                  // Device header
#include "selfprintf.h"



//PC2对应ADC1的通道12初始化
void adc1_PC2_ch12_init(void){
	//GPIO口初始化PC2
		//端口时钟使能
		RCC->AHB1ENR |= (1U<<2);
	
		GPIOC->MODER &= ~(3U<<4);
		GPIOC->MODER |= (3U<<4);
	
	//ADC控制器配置
		//外设时钟使能
		RCC->APB2ENR |= (1U<<8);
		//CR1
		ADC1->CR1 &= ~(3U<<24);
		ADC1->CR1 &= ~(1U<<8);//放最后面，非扫描模式
		//CR2
		ADC1->CR2 &= ~(1U<<11);
		ADC1->CR2 |= (1U<<10);
		ADC1->CR2 &= ~(1U<<1);
		//ADC1->CR2 |= (1U<<0);//放最后面，使能转换器
		//SMPRX
		ADC1->SMPR1 &= ~(7U<<6);
		ADC1->SMPR1 |= (7U<<6);
		//SQRX
		ADC1->SQR1 &= ~(15U<<20); 
		ADC1->SQR3 &= ~(0x1f<<0);
		ADC1->SQR3 |= (12U<<0);
		//CCR
		ADC->CCR &= ~(3U<<16);
		
		//ADC1->CR1 &= ~(1U<<8);//放最后面，使能扫描模式
		ADC1->CR2 |= (1U<<0);//放最后面，使能转换器
}

//获取通道12转换后的数据
u16 adc1_PC2_ch12_get(void){
	u16 data = 0;
	//开启转换
	ADC1->CR2 |= (1U<<30);
	//等待转换完成
	while(!(ADC1->SR & (1U<<1)));
	//获取数据并清楚了标志位
	data = ADC1->DR;
	return data;
}

//PC0对应adc2_ch10的初始化
void adc2_PC0_ch10_init(void){
	//GPIO口初始化PC0
		//端口时钟使能
		RCC->AHB1ENR |= (1U<<2);
	
		GPIOC->MODER &= ~(3U<<0);
		GPIOC->MODER |= (3U<<0);
	
	//ADC控制器配置
		//外设时钟使能
		RCC->APB2ENR |= (1U<<9);
		//CR1
		ADC2->CR1 &= ~(3U<<24);
		//ADC2->CR1 &= ~(1U<<8);//放最后面，非扫描模式
		//CR2
		ADC2->CR2 &= ~(1U<<11);
		ADC2->CR2 |= (1U<<10);
		ADC2->CR2 &= ~(1U<<1);
		//ADC2->CR2 |= (1U<<0);//放最后面，使能转换器
		//SMPRX
		ADC2->SMPR1 &= ~(7U<<0);
		ADC2->SMPR1 |= (7U<<0);
		//SQRX
		ADC2->SQR1 &= ~(15U<<20); 
		ADC2->SQR3 &= ~(0x1f<<0);
		ADC2->SQR3 |= (10U<<0);
		//CCR
		ADC->CCR &= ~(3U<<16);
		
		ADC2->CR1 &= ~(1U<<8);//放最后面，使能扫描模式
		ADC2->CR2 |= (1U<<0);//放最后面，使能转换器
}

//获取ADC2通道10转换后的数据
u16 adc2_PC0_ch10_get(void){
	u16 data = 0;
	//开启转换
	ADC2->CR2 |= (1U<<30);
	//等待转换完成
	while(!(ADC2->SR & (1U<<1)));
	//获取数据并清楚了标志位
	data = ADC2->DR;
	return data;
}


//同ADC的多通道使用 加一个温度通道（没有GPIO口，通道16）
//PC0对应adc1_ch10的初始化
//PC2对应ADC1的通道12初始化
void adc1_PC2PC0_ch12ch10ch16_init(void){
	//GPIO口初始化PC0 PC2
		//端口时钟使能
		RCC->AHB1ENR |= (1U<<2);
	
		GPIOC->MODER &= ~(3U<<0 | 3U<<4);
		GPIOC->MODER |= (3U<<0 | 3U<<4);
	
	//ADC控制器配置
		//外设时钟使能
		RCC->APB2ENR |= (1U<<8);
		//CR1
		ADC1->CR1 &= ~(3U<<24);
		ADC1->CR1 |= (1U<<8);//扫描模式
	
		//CR2
		ADC1->CR2 &= ~(1U<<11);
		ADC1->CR2 |= (1U<<10);
		ADC1->CR2 &= ~(1U<<1);
		//ADC2->CR2 |= (1U<<0);//放最后面，使能转换器
	
		//SMPRX（三通道）
		ADC1->SMPR1 &= ~(7U<<0 | 7U<<6 | 7U<<18);
		ADC1->SMPR1 |= (7U<<0 | 7U<<6 | 7U<<18);
		
		//SQRX
		ADC1->SQR1 &= ~(15U<<20);
		ADC1->SQR1 |= (2U<<20);	//总计三次转换	
		
		ADC1->SQR3 &= ~(0x1f<<0);
		ADC1->SQR3 |= (12U<<0);  //先转换12通道，光敏
		
		ADC1->SQR3 &= ~(0x1f<<5);
		ADC1->SQR3 |= (10U<<5);  //再转换10通道，毒气
		
		ADC1->SQR3 &= ~(0x1f<<10);
		ADC1->SQR3 |= (16U<<10);  //再转换16通道，温度
		
		//CCR
		ADC->CCR &= ~(3U<<16);
		ADC->CCR |= (1U<<23);//开启温度通道
		
		ADC1->CR2 |= (1U<<0);//放最后面，使能转换器
		//printf("adc1_PC2PC0_ch12ch10_init初始化完成\r\n");
}

//获取ADC1通道10 12转换后的数据以及温度
ADC1_t adc1_PC2PC0_ch12ch10ch16_get(void){
	ADC1_t adc1_data;
	//开启转换
	ADC1->CR2 |= (1U<<30);
	//等待转换完成
	while(!(ADC1->SR & (1U<<1)));
	//先获取光敏数据
	adc1_data.lightData = ADC1->DR;
	adc1_data.lightData = (4095-adc1_data.lightData)/4095.0*100;
	
	//等待转换完成
	while(!(ADC1->SR & (1U<<1)));
	//获取气体数据
	adc1_data.gasData = ADC1->DR;
	adc1_data.gasData = adc1_data.gasData/4095.0*100;
	
	//等待转换完成
	while(!(ADC1->SR & (1U<<1)));
	//获取温度数据
	adc1_data.temp = ADC1->DR;
	adc1_data.temp = (adc1_data.temp*0.805-760)/2.5+25;
	
//	printf("返回单次三通道转换的数据/r/n");
//	printf("%d %d %f\r\n",adc1_data.lightData,adc1_data.gasData,adc1_data.temp);
	return adc1_data;
}

//使用中断方式作为三通道转换数据获取时机(上面是查询等待)
void adc1_interrupt_init(void){
	//GPIO口初始化PC0 PC2
		//端口时钟使能
		RCC->AHB1ENR |= (1U<<2);
	
		GPIOC->MODER &= ~(3U<<0 | 3U<<4);
		GPIOC->MODER |= (3U<<0 | 3U<<4);
	
	//ADC控制器配置
		//外设时钟使能
		RCC->APB2ENR |= (1U<<8);
		//CR1
		ADC1->CR1 &= ~(3U<<24);
		ADC1->CR1 |= (1U<<8);//扫描模式
		//ADC1->CR1 |= (1U<<5);//转换标识置1时触发中断
	
		//CR2
		ADC1->CR2 &= ~(1U<<11);
		ADC1->CR2 |= (1U<<10);
		ADC1->CR2 &= ~(1U<<1);
		//ADC2->CR2 |= (1U<<0);//放最后面，使能转换器
	
		//SMPRX（三通道）
		ADC1->SMPR1 &= ~(7U<<0 | 7U<<6 | 7U<<18);
		ADC1->SMPR1 |= (7U<<0 | 7U<<6 | 7U<<18);
		
		//SQRX
		ADC1->SQR1 &= ~(15U<<20);
		ADC1->SQR1 |= (2U<<20);	//总计三次转换	
		
		ADC1->SQR3 &= ~(0x1f<<0);
		ADC1->SQR3 |= (12U<<0);  //先转换12通道，光敏
		
		ADC1->SQR3 &= ~(0x1f<<5);
		ADC1->SQR3 |= (10U<<5);  //再转换10通道，毒气
		
		ADC1->SQR3 &= ~(0x1f<<10);
		ADC1->SQR3 |= (16U<<10);  //再转换16通道，温度
		
		//CCR
		ADC->CCR &= ~(3U<<16);
		ADC->CCR |= (1U<<23);//开启温度通道
		
		//片上外设还需要NVIC来管理和使能中断
		// 1. 设置优先级分组为分组（只有一个，放在主函数）
		//NVIC_SetPriorityGrouping(5); // 抢占优先级 2 位，响应优先级 2 位

		// 2. 计算优先级编码值
		u32 pri = NVIC_EncodePriority(5, 0, 0); // 分组值5，抢占优先级 0，响应优先级 0

		// 3. 设置到具体的中断源 ADC_IRQn 的优先级
		//共享的中断通道，记得在中断服务函数中判断一下是什么中断信号
		NVIC_SetPriority(ADC_IRQn, pri);

		// 4. 启用 ADC_IRQn 中断，三个ADC控制器共用
		NVIC_EnableIRQ(ADC_IRQn);
		
		ADC1->CR1 |= (1U<<5);//转换标识置1时触发中断
		ADC1->CR2 |= (1U<<0);//放最后面，使能转换器
		//printf("三通道中断配置初始化完成\r\n");
}