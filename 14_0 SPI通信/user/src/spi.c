#include "spi.h"
#include "stm32f4xx.h"                  // Device header




//spi所用IO端口初始化，直接使用GPIO口模拟，不通过片上外设对外交流,通用模式
/*
SCK-----PA5     SPI1_SCK  通用输出 发送
MOSI----PA7     SPI1_MOSI 通用输出 发送
CS------PB14    SPI1_NSS  通用输出 片选
MISO----PA6     SPI1_MISO 通用输入 接收
*/
void spi1_init(void){
    //GPIO总线的时钟使能
    RCC->AHB1ENR |= (1<<0) | (1<<1);
    //GPIO模式配置
    GPIOA->MODER &= ~((3U<<10) | (3U<<14));
    GPIOA->MODER |= (1U<<10) | (1U<<14);
    GPIOA->MODER &= ~(3U<<12);
    //GPIO输出配置
    GPIOA->OTYPER &= ~(1U<<5 | 1U<<7);//推挽输出
    //GPIO输出速度配置
    GPIOA->OSPEEDR &= ~((3U<<10) | (3U<<14));
    GPIOA->OSPEEDR |= (2U<<10) | (2U<<14);//50MHz
    //GPIO输出配置
    GPIOA->PUPDR &= ~((3U<<10) | (3U<<12) | (3U<<14));
		
		// 配置PB14为从设备片选引脚（通用输出模式）
    GPIOB->MODER &= ~(3U<<28);  // 清除PB14原配置
    GPIOB->MODER |= (1U<<28);   // 配置为通用输出模式
    GPIOB->OTYPER &= ~(1U<<14); // 推挽输出
    GPIOB->OSPEEDR &= ~(3U<<28);// 清除速度配置
    GPIOB->OSPEEDR |= (1U<<29); // 设置为50MHz
    GPIOB->PUPDR &= ~(3U<<28);//无上下拉
    GPIOB->ODR &= ~(1U<<14);    // 初始化拉低片选引脚

    //时钟极性 低 0
    GPIOA->ODR &= ~(1U<<5);
}

//发送一个字节
//时钟极性 0 低
//时钟相位 0 低  第一个跳变沿读数据，时钟一开始是低，固定第一个跳变沿是上升沿，第二个跳变沿是下降沿
u8 spi1_byte(u8 data){
    u8 i;
    for(i=0;i<8;i++){
        //发送一位数据
        SPI_SCLK_L;
        if(data & 0x80)
            SPI_MOSI_H;
        else
            SPI_MOSI_L;
        data <<= 1;
        
        //接收一位数据
        SPI_SCLK_H;
        if(SPI_MISO)
            data |= 0x01;
    }
    return data;
}


/*
SCK-----PA5     SPI1_SCK  复用输出 发送
MOSI----PA7     SPI1_MOSI 复用输出 发送
CS------PB14    SPI1_NSS  通用输出 片选
MISO----PA6     SPI1_MISO 复用输入 接收
*/
//SPI控制器配置的初始化
void spi1_controller_init(void){
    //GPIO总线时钟使能
    RCC->AHB1ENR |= (1U<<0);//使能GPIOA

    //GPIO复用模式配置
    GPIOA->MODER &= ~((3U<<10) | (3U<<12) | (3U<<14)); //清除原配置
    GPIOA->MODER |= (2U<<10) | (2U<<12) | (2U<<14);    //设置为复用模式
    //复用输出类型 PA5 PA7
    GPIOA->OTYPER &= ~((1U<<5) | (1U<<7)); // 推挽输出
    //复用输出速度
    GPIOA->OSPEEDR &= ~((3U<<10) | (3U<<14));// 清除速度配置
    GPIOA->OSPEEDR |= ((1U<<11) | (1U<<15)); // 设置为50MHz
    //复用上下拉
    GPIOA->PUPDR &= ~((3U<<10) | (3U<<12) | (3u<<14));//无上下拉
    
    //配置GPIO的复用功能为SPI AF5
    GPIOA->AFR[0] &= ~((0xF<<20) | (0xF<<24) | (0xF<<28)); 
    GPIOA->AFR[0] |= (5U<<20) | (5U<<24) | (5U<<28);   //配置为AF5（SPI1）
		
		//单独片选线配置，方便移植
//    // 配置PB14为片选引脚（通用输出模式）
//    GPIOB->MODER &= ~(3U<<28);  // 清除PB14原配置
//    GPIOB->MODER |= (1U<<28);   // 配置为通用输出模式
//    GPIOB->OTYPER &= ~(1U<<14); // 推挽输出
//    GPIOB->OSPEEDR &= ~(3U<<28);// 清除速度配置
//    GPIOB->OSPEEDR |= (1U<<29); // 设置为50MHz
//    GPIOB->PUPDR &= ~(3U<<28);//无上下拉
//    // 确保CS引脚初始化为高
//    GPIOB->ODR |= (1U<<14);  // 拉高片选，通信时才拉低。


    //SPI配置
    RCC->APB2ENR |= (1<<12); //SPI1时钟使能
    //CR1寄存器配置
    SPI1->CR1 &= ~(1U<<15); //双线单向
    SPI1->CR1 &= ~(1U<<11); //8位数据帧
    SPI1->CR1 &= ~(1U<<10); //双线单向下，全双工
    SPI1->CR1 |= (1U<<9); //NSS引脚选择软件管理
    SPI1->CR1 |= (1U<<8); //NSS引脚的值置1，允许SPI控制器通信，看框图的值
    SPI1->CR1 &= ~(1U<<7); //先发高位
    SPI1->CR1 &= ~(7U<<3); //波特率控制为FPCLK/2：84/2=42MHz
    SPI1->CR1 |= (1U<<2); //主配置
    SPI1->CR1 &= ~(1U<<1); //CPOL=0 CPHA=0
    SPI1->CR1 &= ~(1U<<0); //CPOL=0 CPHA=0

    //CR2寄存器配置
    SPI1->CR2 &= ~(1U<<4);//SPI Motorola 模式

    SPI1->CR1 |= (1U<<6); //使能SPI
}

//SPI全双工发送和接收一个字节（带有超时机制）
u8 spi1_controller_TransmitReceive(u8 data){
    //u32 timeout = SPI_TIMEOUT;
    //等待发送缓冲区空
    while(!(SPI1->SR & (1U<<1))){
        //if(timeout-- == 0) return 0xFF; // 超时返回
    }
    SPI1->DR = data; //发送数据，发送完数据后，接收缓冲区就有8位数据了
    
    //等待上一次接收缓冲区非空
    //timeout = SPI_TIMEOUT;
    while(!(SPI1->SR & (1U<<0))){
         //if(timeout-- == 0) return 0xFF; // 超时返回
    }
    return SPI1->DR; //返回接收到的数据
}
