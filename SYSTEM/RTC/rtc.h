#ifndef _RTC_H_
#define _RTC_H_

#include "Sys.h"

#define RTC_SOURCE 1


extern RTC_DateTypeDef  RTC_DateStructure;
extern RTC_TimeTypeDef  RTC_TimeStructure;
extern volatile int alarm_ready;


void rtc_init(void);
void rtc_alarm_a_init(void);
void rtc_SetAlarm(uint32_t hours,uint32_t minuts,uint32_t sec);
uint32_t DEC2BCD(uint32_t dec);//最大7位十进制数
uint32_t BCD2DEC(uint32_t bcd);

#endif 
