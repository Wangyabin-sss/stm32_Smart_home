#ifndef _MAIN_H_
#define _MAIN_H_


//freertos���ͷ�ļ�
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "stack_macros.h"
#include "semphr.h"
#include "event_groups.h"
//�������ͷ�ļ�
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
//GUI���ͷ�ļ�
#include "gui.h"
//�ļ�ϵͳ���ͷ�ļ�
#include "ff.h"

typedef struct nrf_data{
	uint8_t buf[32];
}nrf_t;

#define EVENT_GROUP_RTC_WAKEUP 		(1<<0)
#define EVENT_GROUP_MUSIC	 		(1<<1)

extern EventGroupHandle_t	event_group;   //�¼���־��
extern QueueHandle_t       queue_nrf;		

void device_overload(uint8_t *send);

#endif

