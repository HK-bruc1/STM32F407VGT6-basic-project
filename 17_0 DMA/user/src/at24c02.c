#include "at24c02.h"
#include "stm32f4xx.h"                  // Device header
#include "iic.h"
#include "wang_time.h"

//用于连读读存数据的
char at24c02_rec_str[] = {0};

//用于AT24C02的数据保存，用于按键控制保存状态
volatile u8 sava_Flag;

//I2C外设初始化，其实就是通信对应GPIO口的相关初始化
void at24c02_init(){
	iic_io_init();
}



//在at24c02中指定地址写入一个字节数据
//一个内部地址，一个要写的数据，器件地址已经焊死固定了
u8 at24c02_write_byte(u8 inner_addr,u8 data){
	//起始条件
	iic_start();
	//发送器件地址
	iic_send_byte(AT24C02_ADDR_W);
	//检测从机是否收到器件地址
	if(iic_get_ack()){
		//无应答,停止信号
		iic_stop();
		return AT24C02_ERR1;
	}
	
	
	//发送内部空间地址
	iic_send_byte(inner_addr);
	//检测有没有收到内部地址
	if(iic_get_ack()){
		//无应答
		iic_stop();
		return AT24C02_ERR2;
	}
	
	//发送数据
	iic_send_byte(data);
	//检测有没有收到数据
	if(iic_get_ack()){
		//无应答
		iic_stop();
		return AT24C02_ERR3;
	}
	
	//结束信号
	iic_stop();
	//写周期：实际存入eeprom的需要时间
	baseTim6Delay_Ms(5);
	
	return AT24C02_NO_ERR;
}



//读取指定内部空间地址中的数据
//返回被状态占用，采用传入变量地址把读取的值带出
u8 at24c02_read_byte(u8 inner_addr,u8 *data){
	//起始条件
	iic_start();
	
	//发送器件地址
	iic_send_byte(AT24C02_ADDR_W);
	//检测从机是否收到器件地址
	if(iic_get_ack()){
		//无应答
		iic_stop();
		return AT24C02_ERR1;
	}
	
	//发送内部空间地址
	iic_send_byte(inner_addr);
	//检测有没有收到内部地址
	if(iic_get_ack()){
		//无应答
		iic_stop();
		return AT24C02_ERR2;
	}
	
	//起始条件
	iic_start();
	
	//发送器件地址
	iic_send_byte(AT24C02_ADDR_R);
	//检测从机是否收到器件地址
	if(iic_get_ack()){
		//无应答
		iic_stop();
		return AT24C02_ERR1;
	}
	
	//读取一个数据
	*data = iic_rec_byte();
	//发送NACK:告诉从器件不需要再发了
	iic_send_ack(1);
	
	//停止条件
	iic_stop();
	//没有写入数据，不用写周期，读取也不需要
	return AT24C02_NO_ERR;
}



//单页写，最多一次性写八个字节，传入一个首地址：指针,一个写入长度
//页边界需要自己把握，超出会回写页首地址
u8 at24c02_page_write(u8 inner_addr,const char *address,u8 length){
	//先检查是否跨页
	if(inner_addr/8!=(inner_addr+length-1)/8){
		//直接返回
		return AT24C02_OVER;
	}
	
	//拿一个临时变量统计字节数防止页边界溢出
	u8 count = 0;
	
	//起始条件
	iic_start();
	
	//发送器件地址
	iic_send_byte(AT24C02_ADDR_W);
	//检测从机是否收到器件地址
	if(iic_get_ack()){
		//无应答
		iic_stop();
		return AT24C02_ERR1;
	}
	
	//发送内部空间地址
	iic_send_byte(inner_addr);
	//检测有没有收到内部地址
	if(iic_get_ack()){
		//无应答
		iic_stop();
		return AT24C02_ERR2;
	}
	
	//把数据拆开一个一个写入,把尾零也算入一个字节
	while(count < length){
		//写入数据
		iic_send_byte(*address);
		//检测有没有收到数据
		if(iic_get_ack()){
			//无应答
			iic_stop();
			return AT24C02_ERR3;
		}
		//指针向后移动
		address++;
		//发送字节计数
		count++;
	}
	
	//结束信号
	iic_stop();
	//写周期：实际存入eeprom的需要时间，缓冲区的大小为一页，8个字节
	baseTim6Delay_Ms(5);
	
	return AT24C02_NO_ERR;
}



//通过随机读启动顺序读取
//读取的起始地址，读取长度
//这里没有页边界的概念，给长度随便读取，不发NACK到存储器尾巴会回到存储器首继续读
u8 at24c02_page_read(u8 inner_addr,char *data,u8 length){
	//为了数组下标跟地址对应，最开始使用数组接收的。
	//data+=inner_addr;不要为了一个小需求方便而改底层逻辑
	//这行代码会导致读取数据时直接偏移数组指针，这可能会引起读取错误。
	//正确的做法应该是直接读取指定地址的数据，而不是修改传入的指针。
	//这就像是你原本想从数组的第0个位置开始读，但突然把整个数组"往后推"了几个位置。
	//当你读取结构体时，如果使用 data += inner_addr，会导致指针偏移，实际读取的数据位置会发生变化。
	//这会使得读取的数据不再是从结构体起始地址开始，而是从某个偏移后的位置开始，从而导致数据不完整或错误。
	
	//拿一个临时变量统计字节数防止页边界溢出
	u8 count = 0;
	
	
	//起始条件
	iic_start();
	
	//发送器件地址
	iic_send_byte(AT24C02_ADDR_W);
	//检测从机是否收到器件地址
	if(iic_get_ack()){
		//无应答
		iic_stop();
		return AT24C02_ERR1;
	}
	
	//发送内部空间地址
	iic_send_byte(inner_addr);
	//检测有没有收到内部地址
	if(iic_get_ack()){
		//无应答
		iic_stop();
		return AT24C02_ERR2;
	}
	
	//起始条件
	iic_start();
	
	//发送器件地址
	iic_send_byte(AT24C02_ADDR_R);
	//检测从机是否收到器件地址
	if(iic_get_ack()){
		//无应答
		iic_stop();
		return AT24C02_ERR1;
	}
	
	//读取前次length-1次，最后一次单独
	while(count < (length-1)){
		//读取一个数据
		//*data = iic_rec_byte();
		//在 C 语言中，数组名和指针在很多情况下是可以互换的。
		//当你传入一个指针时，实际上可以用数组下标语法来访问和操作数据。
		data[count] = iic_rec_byte();
		//向后偏移一个地址
		//data++;
		//字节数+1
		count++;
		//发送ACK,告诉从机还要读取
		iic_send_ack(0);
	
	}
	
	//读取一个数据
	//*data = iic_rec_byte();
	data[count] = iic_rec_byte();
	//发送ACK,告诉从机还要读取
	iic_send_ack(1);
	
	//停止条件
	iic_stop();
	//没有写入数据，不用写周期，读取也不需要
	return AT24C02_NO_ERR;
}


//跨页写多字节
void at24c02_skip_page_write(u8 inner_addr,const char *address,u8 length){
	u8 less_num;
	while(1){
		//计算本页还有多少字节空间可以写less_num
		less_num = 8 - inner_addr % 8;
		
		//判断要写的字节数据不需要跨页
		if(less_num >= length){
			//调用页写函数，写入即可  inner_addr,address,length
			at24c02_page_write(inner_addr,address,length);
			//不需要换页，直接跳出
			break;
		}
		//要写入的字节数据需要跨页，
		else {
			//调用页写函数先把本页剩余的空间写满
			at24c02_page_write(inner_addr,address,less_num);
			//计算还有多少字节要写
			length = length - less_num;
			//计算剩余数据的传入地址
			address = address + less_num;
			//计算下一页的起始地址
			inner_addr = inner_addr + less_num;
		}
	}
}
