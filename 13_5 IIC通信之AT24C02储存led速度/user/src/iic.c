#include "iic.h"
#include "stm32f4xx.h"                  // Device header
#include "wang_time.h"



//iic通信数据帧的初始化,SCL PB6 SDA PB7
void iic_io_init(void){
	//端口对应时钟使能
	RCC->AHB1ENR |= (1U<<1);
	//都为开漏输出模式,简化配置
	GPIOB->MODER &= ~((3U<<12) | (3U<<14));
	GPIOB->MODER |= ((1U<<12) | (1u<<14));
	
	GPIOB->OTYPER |= ((1U<<6) | (1U<<7));
	
	GPIOB->OSPEEDR &= ~((3U<<12) | (3U<<14));
	
	GPIOB->PUPDR &= ~((3U<<12) | (3U<<14));
	
	//初始状态两线都为高电平,开漏模式下，呈现高阻态
	GPIOB->ODR |= ((1U<<6) | (1U<<7));
}



//iic的起始信号,操作前后都要拉低SCL防止出现重复误识别为起始条件和停止条件
void iic_start(void){
	//SCL在高电平期间SDA出现下降沿
	
	//SCL拉低，安全作用，如果SCL此前为高，SDA为低，SDA拉高会出现上升沿即停止信号
	IIC_SCL_L;
	baseTim6Delay_Us(5);//为了保持周期稳定
	
	
	//SDA拉高
	IIC_SDA_OUT_H;
	baseTim6Delay_Us(5);
	//SCL拉高
	IIC_SCL_H;
	baseTim6Delay_Us(5);
	//SDA拉低
	IIC_SDA_OUT_L;
	baseTim6Delay_Us(5);
	
	
	//SCL再拉低，安全作用，防止后续出现SDA波动，如果SDA拉高，还会出现上升沿
	IIC_SCL_L;
}


//停止信号
void iic_stop(void){
	//SCL在高电平期间SDA出现上升沿
	
	//SCL拉低
	IIC_SCL_L;
	baseTim6Delay_Us(5);
	
	//SDA拉低
	IIC_SDA_OUT_L;
	baseTim6Delay_Us(5);
	//SCL拉高
	IIC_SCL_H;
	baseTim6Delay_Us(5);
	//SDA拉高
	IIC_SDA_OUT_H;
	baseTim6Delay_Us(5);
	
	
	//SCL拉低，不需要了，因为都回到了初始状态,已经出现停止信号，不会波动
	//IIC_SCL_L;
}


//是否发送应答部分
void iic_send_ack(u8 ack){
	//时序图而已，只要有SCL和SDA能对应即可达到效果。
	//在第九个数据传输周期的低电平期间，SDA拉低为应答，拉高为不应答
	//也就是说在第九个数据传输周期的高电平期间，从机 会读取，SDA已经保持了低电平或者保持高电平
	//所以SDA先拉低，再SCL拉高。因为先动了SDA所以之前还要吧SCL拉低
	
	//先SCL拉低
	IIC_SCL_L;
	baseTim6Delay_Us(3);
	//SDA拉低或拉高
	ack ? (IIC_SDA_OUT_H) : (IIC_SDA_OUT_L);
	baseTim6Delay_Us(2);
	
	
	IIC_SCL_H;//帮助从机拉高，方便读取
	baseTim6Delay_Us(5);
	
	//安全作用
	IIC_SCL_L;
}


//检测是否为应答部分
u8 iic_get_ack(void){
	u8 ack;
	
	//先把数据线切换为输入状态，即数据线置1成为高阻态
	IIC_SCL_L;//安全作用
	IIC_SDA_OUT_H;
	
	
	
	//读去应答信号
	//从机需要主机帮助把SCL拉低，以便从机可以发送ACK和NACK信号
	IIC_SCL_L;
	baseTim6Delay_Us(5);
	// 拉高SCL准备读取应答信号
	IIC_SCL_H;
	baseTim6Delay_Us(5);
	
	//读IO口的输入数据寄存器判断值
	if(IIC_SDA_IN){
		//是一个高电平，NACK
		ack = 1;
	}else {
		//是一个低电平，ACK
		ack = 0;
	}
	
	//安全作用
	IIC_SCL_L;
	return ack;
}


//发送一个字节数据
void iic_send_byte(u8 data){
	u8 count = 8;
	// 从最高位(MSB)开始发送
	while(count > 0){
		// SCL先保持低电平，准备数据变化
		IIC_SCL_L;
		baseTim6Delay_Us(3);
		
		
		// 设置数据位
		if(data & (1U << (count-1))){
				IIC_SDA_OUT_H;  // 发送1
		}else{
				IIC_SDA_OUT_L;  // 发送0
		}
		
		baseTim6Delay_Us(2);
		// SCL拉高，从机采样数据
		IIC_SCL_H;
		baseTim6Delay_Us(5);
		
		
		count--;
	}
    
	//安全作用
	IIC_SCL_L;
}


//接收一个字节数据
u8 iic_rec_byte(void){
	u8 data;
	u8 i;
	
	//数据线切换为输入状态
	IIC_SCL_L;
	IIC_SDA_OUT_H;
	
	
	//读取8位数据
	for(i = 0;i<8;i++){
		//主机帮助从机拉低时钟线，从机自动发送一位数据,此拉低不可省略，要拉低8次
		IIC_SCL_L;
		baseTim6Delay_Us(5);
		//时钟线拉高，以便主机读取数据
		IIC_SCL_H;
		baseTim6Delay_Us(5);
		//读取数据线的电平判断是0还是1
		data = data<<1;
		if(IIC_SDA_IN){
			data = data | 0x01;
		}
	}
	
	//安全作用
	IIC_SCL_L;
	return data;
}