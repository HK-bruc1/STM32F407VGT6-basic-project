#include "event_handler.h"
#include "nvic.h"
#include "adc.h"
#include "selfprintf.h"
#include "BEEP.h"
#include "LED.h"
#include "functions.h"


//用作状态机扫描各事件的触发条件
void Scanning_Events(void){
	//毒气和光敏
	u16 LightData;
	u16 GasData;
	u16 brightness;
	//同ADC1不同通道转换的毒气和光敏结构体
	ADC1_t val;
	//气体泄漏标志位
	u8 gasLeak;
	
	//每100ms获取温度，气体，光敏数据并触发相应的事件
	if(TIM7Count[7]>=100){
		TIM7Count[7] = 0;
		val = adc1_PC2PC0_ch12ch10ch16_get();
		//返回的数据已经转换成0~100之间了
		// 将0-100的光照百分比映射到0-1000的LED亮度范围
		brightness = (val.lightData * 10); // 将百分比转换为0-1000范围
		// 设置LED亮度
		TIM3->CCR1 = brightness;
		
		// 气体数据
		//浓度超过20%，才开启电机，浓度越高越快，先初始化tim13_MotorInit
		if(val.gasData > 20){
				//把气体泄漏标志置1
				gasLeak = 1;
				// 将20-100的浓度范围映射到0-10的转速范围
				// 浓度每增加8，速度增加1
				u8 speed = (val.gasData - 20) / 8; 
				
				// 设置电机速度
				TIM13->CCR1 = speed;
				
				printf("毒气浓度：%d%%  电机速度档位：%d\r\n", val.gasData,speed);
		}else{
				//把气体泄漏标志置0
				gasLeak = 0;
				//把蜂鸣器关了，标志位置0，程序进不去，无法关闭
				//BEEP_OFF;
				LED1_OFF;
				// 浓度低于20%时停止电机
				TIM13->CCR1 = 0;
				printf("毒气浓度：%d%%  电机处于关闭状态\r\n",val.gasData);
		}
		
		printf("光照强度百分比：%d%%  LED亮度值：%d\r\n", val.lightData, brightness);
		printf("机器内部温度数据：%.2f°C\r\n",val.temp);
		
		//气体泄漏的一些事件
		//判断是否开启超声波测距
		if(gasLeak==1){
			//开始工作超声波
			SR04_Start();
			//开始工作舵机，不用考虑再打开,先初始化tim14_ServoInit
			TIM14->CCR1 = 125;
		}
	}
}