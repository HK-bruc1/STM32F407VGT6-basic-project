#include "stm32f4xx.h"                  // Device header
#include "rtc.h"
#include "selfprintf.h"

//rtc初始化
void rtc_init(){
	//使能PWR外设时钟
    RCC->APB1ENR |= (1U<<28);
	//RCC_BDCR 寄存器、 RTC 寄存器（包括备份寄存器）以及 PWR_CSR 寄存器的 BRE 位均受到写访问保护
	//因为要修改RCC->BDCR的值，必须要在前面！
		PWR->CR |= (1U<<8);

	//选择rtc时钟来源-备份域控制寄存器 (RCC_BDCR)
		//开启外部低速时钟LSE
		RCC->BDCR |= (1U<<0);
		//等待时钟就绪
		while(!(RCC->BDCR & (1U<<1)));
		//选择RTC时钟源:外部低速时钟
		RCC->BDCR &= ~(3U<<8);
		RCC->BDCR |= (1U<<8);
		//使能RTC
		RCC->BDCR |= (1U<<15);
	
	//解除TRC控制器的写保护
	//PWR->CR |= (1U<<8);
	//解除寄存器的写保护
		RTC->WPR = 0xCA;
		RTC->WPR = 0x53;
	
	//日历初始化设置日期和时间(初始化带的)
		//开启初始化模式
		RTC->ISR |= (1U<<7);
		//等待初始化完成
		while(!(RTC->ISR & (1U<<6)));
		//系统计算时间格式为24小时制
		RTC->CR &= ~(1U<<6);
		//激活影子寄存器
		RTC->CR &= ~(1U<<5);
		//预分频配置默认就是128，同步分频默认就是256。1HZ
		//设置时间 因为 "|=" 多余的也不会改变原值,会改变，用了赋值符号
		//使用备份域寄存器的特性作为标志，只要不是全部掉电，就只初始化一次时间即可
		//其他部件功能还是需要初始化
		if(RTC->BKP0R!=800){
			//写入的时间格式是24小时制
			RTC->TR &= ~(1U<<22);
			// 设置时间
			RTC->TR = (0x14 << 16) | (0x13 << 8) | (0x13 << 0); // 小时:13, 分:13, 秒:13
			// 设置日期
			RTC->DR = (0x12 << 16) | (0x7 << 13) | (0x12 << 8) | (0x12 << 0); // 年:12, 星期:7, 月:12, 日:12
			//全部掉电后重新上电是0x0000 0000
			RTC->BKP0R = 800;
		}

		//退出初始化模式
		RTC->ISR &= ~(1U<<7);
		while (RTC->ISR & (1U << 6)); // 确保INITF清零
	
	//激活寄存器的写保护
	RTC->WPR = 0xFF;
}


//数据分离思想
//两位数十进制转BCD码
u8 in_dec_out_bcd(u8 dec){
	return ((dec/10)<<4) | (dec%10); 
}


//BCD码转两位十进制数
u8 in_bcd_out_dec(u8 bcd){
	return (bcd >> 4)*10 + (bcd & 0x0f);
}


//单独开一个时间和日期的设置避免设置初始化（把初始化一部分拿出来单独封装）
void rtc_set(RTC_T data){
	//组合数据的临时变量
	u32 temp_d = 0;
	u32 temp_t = 0;
	//解除寄存器写保护（控制器写保护已经在初始化中开启并没有关闭）
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
	//让日历计数器进入初始化模式
	RTC->ISR |= (1U<<7);
	//等待初始化完成
	while(!(RTC->ISR & (1U<<6)));
	//把十进制数据转换为BCD码
	temp_d = (in_dec_out_bcd(data.years-2000)<<16)|
					 (in_dec_out_bcd(data.week)<<13)|
					 (in_dec_out_bcd(data.mon)<<8)|
					 (in_dec_out_bcd(data.date));
	
	temp_t = (in_dec_out_bcd(data.h)<<16)|
					 (in_dec_out_bcd(data.m)<<8)|
					 (in_dec_out_bcd(data.s));
	
	//统一设置时间和日期
	RTC->DR = temp_d;
	RTC->TR = temp_t;
	
	//退出初始化模式
	RTC->ISR &= ~(1U<<7);
	
	//激活寄存器的写保护
	RTC->WPR = 0xFF;
}


//读取日历
RTC_T rtc_get(void){
	//组合数据的临时变量
	u32 temp_d = 0;
	u32 temp_t = 0;
	RTC_T data = {0};
	
	//软件必须分两次读取日历时间寄存器和日期寄存器
	//为了确保是最新的时间需要先把标志位清零
	RTC->ISR &= ~(1U<<5);
	//等待数据加载到影子寄存器
	while(!(RTC->ISR & (1U<<5)));
	//读取日期
	temp_d = RTC->DR;
	//手动清零再等待一次
	RTC->ISR &= ~(1U<<5);
	while(!(RTC->ISR &= ~(1U<<5)));
	//读取时间
	temp_t = RTC->TR;
	
	//拆分数据开始赋值结构体
	data.years = in_bcd_out_dec((temp_d >> 16) & 0xff) + 2000;
	data.week = in_bcd_out_dec((temp_d >> 13) & 0x7);
	data.mon = in_bcd_out_dec((temp_d >> 8) & 0x1f);
	data.date = in_bcd_out_dec((temp_d >> 0) & 0x3f);
	
	data.h = in_bcd_out_dec((temp_t >> 16) & 0x3f);
	data.m = in_bcd_out_dec((temp_t >> 8) & 0x7f);
	data.s = in_bcd_out_dec((temp_t >> 0) & 0x7f);
	
	return data;
}