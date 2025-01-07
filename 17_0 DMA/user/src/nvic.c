#include "nvic.h"
#include "stm32f4xx.h"                  // Device header
#include "selfprintf.h"
#include "systick.h"
#include "wang_time.h"
#include "LED.h"
#include "GeneralTim.h"
#include "BEEP.h"
#include "adc.h"
#include "functions.h"



//定义一个结构体变量并自动初始化
USART1BAL_t usart1_val;

//把单独一个标识扩展到全局,以便住主函数判断
volatile u8 countFlag;
//全局计数，中断尽量短而快
volatile u32 count;

//以1ms中断一次，用数组作事件单独判断
volatile u32 sysTickCount[5];

//以1ms中断一次，用数组作事件单独判断
volatile u32 TIM7Count[15];

//以1ms中断一次，用数组作事件单独判断
volatile u32 TIM6Count[5];

//给一个标志位代表红外感应是否有障碍
volatile u8 IR_statu;

//定时中断流水灯速度控制(0~10)
volatile u8 Tim7WaterLed_Speed;
volatile u8 waterLedOpen_Flag;




//串口1接收完成中断处理函数
void USART1_IRQHandler(void) {
    //USART1 中断处理函数
		//接收一个字节数据完成触发中断
    if (USART1->SR & (1U<<5)) {
        usart1_val.usart1_buff[usart1_val.length] = USART1->DR; //通过对 USART_DR 寄存器执行读入操作将该中断状态位清零
				usart1_val.length++;
    }
		
		//当传输多个字节数据后，线路空闲时触发中断
		//该位由软件序列清零（读入 USART_SR 寄存器，然后读入 USART_DR 寄存器）
		if(USART1->SR & (1U<<4)){
				//清除标志位
				//USART1->SR;
				USART1->DR;
				//在这里把缓冲区数组最后面加一个'\0'
				usart1_val.usart1_buff[usart1_val.length] = '\0';
				//重置缓冲区下标
				usart1_val.length = 0;
				//会自动检测尾巴0结束符号
				printf("usart1_buff:\t%s\r\n",usart1_val.usart1_buff);
				//给一个字符串接收完成的标识，定义全局变量，用于外部判断
				usart1_val.usart1StrRec_flag = 1;
		}
}


//EXTI8中断服务函数
void EXTI9_5_IRQHandler(void){
	//给一个临时静态局部变量用于计数
	//static u32 count = 0;
	//做一个标识，用作消抖使用
	static u8 extiFlag = 1;
	
	//共用的线，先判断具体是哪一个中断信号
	if(EXTI->PR & (1<<8)){
		//先清除状态位,置1清除状态
		EXTI->PR |= (1<<8);
		//按下和松开都会进入中断
		delay_ms(15);
		//直接访问寄存器快得多，不想用函数了。。。
		if((!(GPIOC->IDR & (1<<8))) && extiFlag){
			//紧急事件
			LED1_TURN;
			count++;
			//printf("物件数量:%d\r\n",count);
			//标识位锁住，在松开之前，紧急事件不会执行第二次
			extiFlag = 0;
			countFlag = 1;
		}else if((GPIOC->IDR & (1<<8)) && extiFlag==0){
			//按住进来一次，状态被清除，除开机械抖动不会在进来
			//15ms后如果机械抖动再进来，标识位锁住了，直接出去（小概率）
			//松开时会进来一次，把标识位解锁
			extiFlag = 1;
			//15ms后如果机械抖动,也进不来，条件不满足
		}
	}
}


//滴答定时中断服务函数(虽然相当于独立的，但是还是写在一起)
void SysTick_Handler(void){
	//清空当前值寄存器,清零对应状态位
	//不想外部知道就不用清零
	SysTick->VAL = 0;
	//打印用
	sysTickCount[0]++;
	//延时函数用，无符号int类型的数据，减到0之后，会环形结构，不会出现溢出的未定义行为
	sysTickCount[1]--;
	
	if(sysTickCount[0]==500){
		//计数器递减到0就触发一次中断
		printf("系统滴答定时中断测试\r\n");
		sysTickCount[0]=0;
	}
}


//基本定时器TIM6的中断服务函数
void TIM6_DAC_IRQHandler(void){
	
	//共享的中断通道先判断是什么中断信号
	if(TIM6->SR & (1U<<0)){
		//清除标志位,这里进入中断需要这个标志位，所以必须要清0
		//和系统滴答定时器不一样
		TIM6->SR &= ~(1U<<0);
		
		//打印用
		TIM6Count[0]++;
		//延迟函数用(at24c02使用了阻塞式没办法再用)
		//TIM6Count[1]--;
		
		
		if(TIM6Count[0]==300){
			printf("基本定时器6定时中断测试\r\n");
			TIM6Count[0]=0;
		}
	}
	
}


//基本定时器TIM7的中断服务函数
void TIM7_IRQHandler(void){
	//各局部变量
	//用于led呼吸的标志位
	static  u8 ledBreathFlag = 1; 
	//用于流水灯编号控制
	static u8 ledNum = 1;
	//毒气和光敏
	u16 LightData;
	u16 GasData;
	u16 brightness;
	//同ADC1不同通道转换的毒气和光敏结构体
	ADC1_t val;
	
	if(TIM7->SR & (1U<<0)){
		//清除标志位,这里进入中断需要这个标志位，所以必须要清0
		//和系统滴答定时器不一样
		TIM7->SR &= ~(1U<<0);

		//紧急事件
		//测试打印用
		//TIM7Count[0]++;
		//LED3呼吸灯用
		//TIM7Count[1]++;
		//流水灯计数使用
		//TIM7Count[2]++;
		//红外感应使用
		//TIM7Count[3]++;
		//实现周期性超声波测量
		//TIM7Count[4]++;
		//用于光敏传感器的周期性检测 100ms
		//TIM7Count[5]++;
		//用于毒气检测
		//TIM7Count[6]++;
		//同时用于光敏和毒气以及温度
		//TIM7Count[7]++;
		//三通道中断方式，周期性开启转换器
		//TIM7Count[8]++;
		//非阻塞式延时用(到达最大事件数)
		TIM7Count[9]--;
		
		//测试中断是否进入
		if(TIM7Count[0]==300){
			printf("基本定时器7定时中断测试\r\n");
			TIM7Count[0]=0;
		}
		
		//LED3呼吸灯
		if(TIM7Count[1]==2){
			if(ledBreathFlag==1){
				TIM3->CCR1++;
				if(TIM3->CCR1 >= 1000){
					ledBreathFlag = 0;
				}
			}else if(ledBreathFlag==0){
				TIM3->CCR1--;
				if(TIM3->CCR1 < 1){
					ledBreathFlag = 1;
				}
			}
			//每一个亮度维持2ms
			TIM7Count[1]=0;
		}
		
		//判断是否开启流水灯
		if(waterLedOpen_Flag){
			//开始计数，开启流水灯
			TIM7Count[2]++;
		}
		
		// 流水灯控制(要初始化led),在外面加一层速度判断
		u8 delay = 80 - (Tim7WaterLed_Speed * 5);
		if((TIM7Count[2] >= delay) && Tim7WaterLed_Speed<=10 && Tim7WaterLed_Speed>=0){ // speed为30ms最快，每次调整以5ms为单位，效果明显
				// 关闭当前LED
				ledX_off(ledNum);
				// 移动到下一个LED
				ledNum = (ledNum % 4) + 1;
				// 打开新的LED
				ledX_on(ledNum);
				TIM7Count[2] = 0;
		}
		
		//红外感应每3ms就检查一次状态
		if(TIM7Count[3]==3){
			TIM7Count[3]=0;
			if(GPIOC->IDR & (1<<8)){
				IR_statu = 0;
			}else {
				IR_statu = 1;
			}
		}
		
		//每100ms超声波测量一次
		if(TIM7Count[4]==3000){
			TIM7Count[4] = 0;
			SR04_Start();
		}
		
		//每100ms光敏传感器转换一次
		if(TIM7Count[5]==100){
			TIM7Count[5] = 0;
			// 获取ADC值并转换为0-100的百分比
			LightData = adc1_PC2_ch12_get();
			LightData = (4095-LightData)/4095.0*100;
			// 将0-100的光照百分比映射到0-1000的LED亮度范围
			brightness = (LightData * 10); // 将百分比转换为0-1000范围
			// 设置LED亮度
			TIM3->CCR1 = brightness;
			printf("光照强度百分比：%d%%  LED亮度值：%d\r\n", LightData, brightness);
		}
		
		//每100ms毒气检测一次
		if(TIM7Count[6]==100){
			TIM7Count[6] = 0;
			// 获取ADC值并转换为0-100的百分比
			GasData = adc2_PC0_ch10_get();
			GasData=GasData/4095.0*100;
			//浓度超过20%，才开启电机，浓度越高越快，先初始化tim13_MotorInit
			if(GasData > 20){
					// 将20-100的浓度范围映射到0-10的转速范围
					// 浓度每增加8，速度增加1
					u8 speed = (GasData - 20) / 8; 
					
					// 设置电机速度
					TIM13->CCR1 = speed;
					
					printf("毒气浓度：%d%%  电机速度档位：%d\r\n", GasData,speed);
			}
			else{
					// 浓度低于20%时停止电机
					TIM13->CCR1 = 0;
					printf("毒气浓度：%d%%  电机处于关闭状态\r\n",GasData);
			}
		}
		
		//定时性开启转换器 每1秒开启一次，一次转换三个通道，数据由触发中断获取
		if(TIM7Count[8]==1000){
			TIM7Count[8] = 0;
			//开启转换
			//printf("开启转换器\r\n");
			ADC1->CR2 |= (1U<<30);
		}
		
		//管理RGB的随机变色的持续时间
		if(randomColorFlag==1){
			//管理RGB的随机变色的持续时间
			TIM7Count[9]++;
			//变色持续500ms就再换一次颜色
			if(TIM7Count[9]==500){
				TIM7Count[9] = 0;
				changeColorRandomly();
			}
		}
		
	}
}


//通用定时器5的中断服务函数
void  TIM5_IRQHandler(void){
	static u16 time_n = 0;
	static u16 test_1 = 0;
	u16 test_2 = 0;
	u32 test = 0;
	
	//判断是哪一个中断信号
	//判断是更新中断,计数器溢出
	if(TIM5->SR & (1U<<0)){
		//清除标志位
		TIM5->SR &= ~(1U<<0);
		time_n++;
	}
	
	//判断是捕获中断
	if(TIM5->SR & (1U<<1)){
		//清除标志位
		TIM5->SR &= ~(1U<<1);
		//紧急事件
		//如果是上升沿,因为初始化时定义了，所以第一次触发必定是设置的那个，所以可以通过判断
		if(!(TIM5->CCER & (1U<<1))){
			time_n = 0;//开始记录有效次数
			//获取捕获寄存器中的值
			test_1 = TIM5->CCR1;
			//切换边沿2
			TIM5->CCER |= (1U<<1);
		}
		//如果是下降沿捕获
		else if(TIM5->CCER & (1U<<1)){
			//记录捕获寄存器的第二次值
			test_2 = TIM5->CCR1;
			//计算脉冲宽度
			test = time_n*65535-test_1+test_2;
			printf("按键按下的时间：%0.1fms\r\n",test/1000.0);
			//再次切换边沿1
			TIM5->CCER &= ~(1U<<1);
		}
	}
}

//通用定时器4的中断服务函数
void  TIM4_IRQHandler(void){
	static u16 time_n = 0;
	static u16 test_1 = 0;
	u16 test_2 = 0;
	u32 test = 0;
	
	//判断是哪一个中断信号
	//判断是更新中断,计数器溢出
	if(TIM4->SR & (1U<<0)){
		//清除标志位
		TIM4->SR &= ~(1U<<0);
		time_n++;
	}
	
	//判断是捕获中断
	if(TIM4->SR & (1U<<1)){
		//清除标志位
		TIM4->SR &= ~(1U<<1);
		//紧急事件
		//如果是上升沿,因为初始化时定义了，所以第一次触发必定是设置的那个，所以可以通过判断
		if(!(TIM4->CCER & (1U<<1))){
			time_n = 0;//开始记录有效次数
			//获取捕获寄存器中的值
			test_1 = TIM4->CCR1;
			//切换边沿2
			TIM4->CCER |= (1U<<1);
		}
		//如果是下降沿捕获
		else if(TIM4->CCER & (1U<<1)){
			//记录捕获寄存器的第二次值
			test_2 = TIM4->CCR1;
			//计算脉冲宽度 单位是us
			test = time_n*65535-test_1+test_2;
			float distance_cm = (34*test/1000.0)/2;
			//小于50cm就报警
			if(distance_cm<=50){
				//BEEP_ON;
				LED1_ON;
				printf("有害气体泄漏请退出50cm以外！当前距离为：%.1fcm\r\n",distance_cm);
			}else {
				//BEEP_OFF;
				LED1_OFF;
			}
			//再次切换边沿1
			TIM4->CCER &= ~(1U<<1);
		}
	}
}


//ADC中断服务函数
void ADC_IRQHandler(void){
	//printf("进入ADC中断\r\n");
	static u8 count = 0;
	static ADC1_t val;
	//判断具体是那个ADC控制器
	if((ADC1->SR & (1U<<1))>>1){
		//printf("进入ADC1中断\r\n");
		count++;
		//printf("之前的值%d\r\n",(ADC1->SR & (1U<<1))>>1);
		//光敏数据
		if(count == 1){
			val.lightData = ADC1->DR;
			val.lightData = (4095-val.lightData)/4095.0*100;
			//printf("%d\r\n",(ADC1->SR & (1U<<1))>>1);
		}else if(count == 2){//气体数据
			val.gasData = ADC1->DR;
			val.gasData = val.gasData/4095.0*100;
		}else if(count == 3){
			count=0;
			val.temp = ADC1->DR;//温度
			val.temp = (val.temp*0.805-760)/2.5+25;
			//判断各数据
			if(val.lightData>80){
				//led3的亮度拉满
				TIM3->CCR1 = 1000;
			}else if(val.lightData<70 && val.lightData>40){
				//led3的亮度减一半
				TIM3->CCR1 = 500;
			}else{
				//led3的亮度最低
				TIM3->CCR1 = 100;
			}
			
			if(val.gasData>50){
				//蜂鸣器响
				BEEP_ON;
			}else{
				//蜂鸣器关
				BEEP_OFF;
			}
			printf("光照强度百分比：%d%%\r\n", val.lightData);
			printf("空气污染程度百分比：%d%%\r\n",val.gasData);
			printf("温度数据：%.2f°C\r\n",val.temp);
		}
	}
}

