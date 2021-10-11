#ifndef _MAIN_H_
#define _MAIN_H_


//freertos相关头文件
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "stack_macros.h"
#include "semphr.h"
#include "event_groups.h"
//外设相关头文件
#include "Sys.h"
#include "fsmc.h"
#include "usart.h"
#include "ov2640.h"
#include "sdio_sdcard.h"
#include "vs10xx.h"
#include "rtc.h"
#include "xpt2046.h"
#include "nrf24l01.h"
#include "mqtt.h"
//GUI相关头文件
#include "gui.h"
//文件系统相关头文件
#include "ff.h"

typedef struct nrf_data{
	uint8_t buf[32];
}nrf_t;

#define EVENT_GROUP_RTC_WAKEUP 		(1<<0)
#define EVENT_GROUP_MUSIC	 		(1<<1)

extern EventGroupHandle_t	event_group;   //事件标志组
extern QueueHandle_t       queue_nrf;		

void device_overload(uint8_t *send);

#endif

