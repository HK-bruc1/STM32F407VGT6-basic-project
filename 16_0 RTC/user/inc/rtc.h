#ifndef RTC_H
#define RTC_H

#include "stm32f4xx.h"                  // Device header

//用于参数传递
typedef struct rtc
{
	u16 years;
	u8 mon;
	u8 date;
	u8 week;
	u8 h;
	u8 s;
	u8 m;
	
}RTC_T;

void rtc_init();
u8 in_dec_out_bcd(u8 dec);
u8 in_bcd_out_dec(u8 bcd);
void rtc_set(RTC_T data);
RTC_T rtc_get(void);





#endif