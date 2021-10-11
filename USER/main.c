#include "main.h"

/********************************������*********************************/
static 	TaskHandle_t	app_task_init_handler		=NULL; 	//��ʼ������
static 	TaskHandle_t	app_task_lvgl_handler		=NULL; 	//gui������
static 	TaskHandle_t	app_task_nrf24l01_handler	=NULL;	//2.4g������
static 	TaskHandle_t	app_task_mqtt_handler		=NULL;	//mqtt������
static 	TaskHandle_t	app_task_rtc_handler		=NULL;	//rtc������
static 	TaskHandle_t	app_task_music_handler		=NULL;	//music������
/********************************������********************************/
static void app_task_init			(void *pvParameters);//��ʼ������
static void app_task_lvgl			(void *pvParameters);//gui������
static void app_task_nrf24l01		(void *pvParameters);//2.4g������
static void app_task_mqtt			(void *pvParameters);//mqtt������
static void app_task_rtc			(void *pvParameters);//rtc������
static void app_task_music			(void *pvParameters);//music������
/********************************ȫ�ֱ���****************************/
EventGroupHandle_t	event_group;   //�¼���־��
QueueHandle_t       queue_nrf;    //nrf��Ϣ����


static uint32_t WIFI_STATUS=0,flame = 0,gas = 0;
static float temperature,humidity;
uint8_t databuf[4096];

static FATFS fsObject;
static FIL fp;
static UINT rw;
static FRESULT fat_res;
static char fat_data[16];

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	SysTick_Config(SystemCoreClock/configTICK_RATE_HZ);
	usart1_init(115200);
	usart2_init(115200);
	//usart3_init(115200);
	
	//������ʼ����
    xTaskCreate((TaskFunction_t ) app_task_init,          //������
				(const char*    ) "start_task",        	  //��������
				(uint16_t       ) 1024,                    //�����ջ��С(��λ��)
				(void*          ) NULL,                   //���ݸ��������Ĳ���
				(UBaseType_t    ) 15,                      //�������ȼ�
				(TaskHandle_t*  ) &app_task_init_handler); //������
				
    vTaskStartScheduler(); //�����������
    while(1);
}



//��ʼ����������
void app_task_init(void *pvParameters)
{
	int ret_val=-1,tmp=0;
	char buf[50];
	lv_obj_t *textarea;
	
	event_group   = xEventGroupCreate();
	queue_nrf 	  = xQueueCreate(5,32);
	
	rtc_init();
	lv_init();
	lv_port_disp_init();   // ��ʾ����ʼ��
	lv_port_indev_init();
	//lv_gui_init();
	textarea = lv_textarea_create(lv_scr_act(), NULL);
    lv_obj_set_size(textarea, 300, 400);
    lv_obj_align(textarea, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_textarea_set_text(textarea, "init msg\n");    /*Set an initial text*/
	
	spi3_init();	//vs1053 & nrf24l01
	VS_Init();
	
	NRF24L01_GPIO_Init();
	NRF24L01_SPI_Init();
	sprintf(buf,"NRF24L01_Check = %d\n",NRF24L01_Check());
	printf("%s",buf);
	lv_textarea_add_text(textarea,buf);
	
	if(SD_Init())//��ⲻ��SD��
	{
		sprintf(buf,"please check sd card\n");
		lv_textarea_add_text(textarea,buf);
	}else{
		lv_textarea_add_text(textarea,"sd card ok\n");
	}
	
	fat_res = f_mount(&fsObject, "1:", 1);	//����sd��
	sprintf(buf,"f_mount res = %d\n", fat_res);
	lv_textarea_add_text(textarea,buf);
	
	fat_res = f_open(&fp, "1:wifi.txt",FA_READ);
	if(fat_res == 0)
	{
		f_gets((void *)fat_data, sizeof(fat_data),&fp); //��ȡһ��
		fat_data[strlen(fat_data)-2] = 0;
		strcpy((char *)WIFI_NAME,fat_data);
		f_gets((void *)fat_data, sizeof(fat_data),&fp); //��ȡһ��
		fat_data[strlen(fat_data)-2] = 0;
		strcpy((char *)WIFI_PWD,fat_data);
	}
	f_close(&fp);
	
	printf("WIFI_NAME=%s,WIFI_PWD=%s\n",WIFI_NAME,WIFI_PWD);
	sprintf(buf,"WIFI_NAME=%s,WIFI_PWD=%s\n",WIFI_NAME,WIFI_PWD);
	lv_textarea_add_text(textarea,buf);
	
	taskENTER_CRITICAL(); //�����ٽ���
	//����GUI����
	xTaskCreate((TaskFunction_t )app_task_lvgl,
				(const char*    )"app_task_lvgl",
				(uint16_t       )512,
				(void*          )NULL,
				(UBaseType_t    )10,
				(TaskHandle_t*  )&app_task_lvgl_handler);
	//����nrf24l01����
	xTaskCreate((TaskFunction_t )app_task_nrf24l01,
				(const char*    )"app_task_nrf24l01",
				(uint16_t       )512,
				(void*          )NULL,
				(UBaseType_t    )8,
				(TaskHandle_t*  )&app_task_nrf24l01_handler);	
	//����mqtt����
	xTaskCreate((TaskFunction_t )app_task_mqtt,
				(const char*    )"app_task_mqtt",
				(uint16_t       )1024,
				(void*          )NULL,
				(UBaseType_t    )8,
				(TaskHandle_t*  )&app_task_mqtt_handler);	
	//����rtc����
	xTaskCreate((TaskFunction_t )app_task_rtc,
				(const char*    )"app_task_rtc",
				(uint16_t       )512,
				(void*          )NULL,
				(UBaseType_t    )8,
				(TaskHandle_t*  )&app_task_rtc_handler);	
	//����music����
	xTaskCreate((TaskFunction_t )app_task_music,
				(const char*    )"app_task_music",
				(uint16_t       )512,
				(void*          )NULL,
				(UBaseType_t    )8,
				(TaskHandle_t*  )&app_task_music_handler);
				
	taskEXIT_CRITICAL(); //�˳��ٽ���
	
	vTaskSuspend(app_task_mqtt_handler);		//wifi״̬��δ�ж�
	vTaskSuspend(app_task_nrf24l01_handler);	//wifi״̬��δ�ж�
	vTaskSuspend(app_task_rtc_handler); //gui��δ��ʼ��
				
	vTaskDelay(1000);
	ret_val = exit_transport();   //esp8266״̬���
	sprintf(buf,"exit_transport = %d\n",ret_val);
	printf("%s",buf);
	lv_textarea_add_text(textarea,buf);
	if(ret_val == 0)
	{
		ret_val = wifi_check();
		if(ret_val == -1)
		{
			//printf("wifi����ʧ��\n");
			lv_textarea_add_text(textarea,"wifi connect error\n");
			do
			{
				tmp++;
				sprintf(buf,"Try to reconnect %d\n",tmp);
				lv_textarea_add_text(textarea,buf);
				ret_val = connent_wifi(); //����wifi
				printf("ret_val:%d\n",ret_val);
			}while(tmp<=3&&ret_val==-1);
			
			if(ret_val == -1)	  //����wifiʧ��
			{
				lv_textarea_add_text(textarea,"WiFi name&password error\nPlease check\n");
				
			}else{
				printf("wifi connect success0\n");
				WIFI_STATUS = 1;
				lv_textarea_add_text(textarea,"wifi connect success0\n");
			}
		}else{
			printf("wifi connect success1\n");
			WIFI_STATUS = 1;
			lv_textarea_add_text(textarea,"wifi connect success1\n");
		}
	}
	
	lv_obj_del(textarea);
	lv_gui_init();
	
	if(WIFI_STATUS) //���wifi
	{
		vTaskResume(app_task_mqtt_handler);
		vTaskResume(app_task_nrf24l01_handler);
	}
	
	vTaskResume(app_task_rtc_handler);
	
	vTaskDelete(NULL); //ɾ����������
}

//GUI��������
void app_task_lvgl(void *pvParameters)
{
	while(1)
	{
		lv_task_handler();
		vTaskDelay(5);
	}
}

void device_overload(uint8_t *send)
{
	int i=0,ret;
	send[0]='D'; send[1]='A'; send[2]='T';
	while(1)
	{
		i+=10;
		if(i > 120)	//һ��������12������
			break;
		NRF24L01_TX_Mode(i);
		send[3] = i;
		ret=NRF24L01_TxPacket(send);
		if(ret == TX_OK)
		{
//			NRF24L01_RX_Mode(i);          //�����������Ϊ����
//			vTaskDelay(100);
//			ret=NRF24L01_RxPacket(nrf_recv);
//			if(ret == 0)
//			{
				printf("add:%d\n",i/10);
				tab2_dev_add(i/10,1);
//			}
		}
		else
		{
			printf("dev %d error\n",i/10);
		}
		vTaskDelay(10);
		
	}
}

//2.4g��������  
//	nrf_send[3] 10��20��30Ϊ1��2��3���豸�����������ƣ�12���豸��
//				11��21��31Ϊ1��2��3���豸д
void app_task_nrf24l01(void *pvParameters)
{
	int ret,freq=0;
	uint8_t nrf_send[32];
	uint8_t nrf_recv[32];
	BaseType_t	xReturn;
	
	device_overload(nrf_send);
	
	while(1)
	{
		xReturn = xQueueReceive(queue_nrf,	    /* ��Ϣ���еľ�� */
								nrf_send, 	    /* �õ�����Ϣ���� */
								portMAX_DELAY);	/* һֱ��*/
		
		if(nrf_send[3]%10) //��ȡ�豸Ƶ��
			freq = nrf_send[3]-1;
		else
			freq = nrf_send[3];
		
		NRF24L01_TX_Mode(freq);
		ret=NRF24L01_TxPacket(nrf_send);
		if(ret == TX_OK)
		{
			NRF24L01_RX_Mode(freq);          //�����������Ϊ����
			memset(nrf_recv,0,32);       //��ս�������
			vTaskDelay(10);
			ret=NRF24L01_RxPacket(nrf_recv);
			if(ret == 0)
			{
				switch (freq) //��ͬ�豸�ֿ�����
				{
					case 10:
						//printf("10,LOCK%d\n",nrf_recv[4]);
						if(nrf_recv[4] == 1)
							xEventGroupSetBits(event_group,EVENT_GROUP_MUSIC);
						break;
					case 20:
						temperature = nrf_recv[4]+nrf_recv[5]*0.01;
						humidity 	= nrf_recv[6]+nrf_recv[7]*0.01;
						flame  		= nrf_recv[8];
						gas			= nrf_recv[9];
						//printf("20,T%.2f,H%.2f,F%d,G%d\n",temperature,humidity,flame,gas);
						updata_thfg(temperature,humidity,flame,gas);
						break;
					case 30:
						break;
					case 40:
						break;
					case 50:
						break;
					case 60:
						break;
					case 70:
						break;
					case 80:
						break;
					case 90:
						break;
					case 100:
						break;
					case 110:
						break;
					case 120:
						break;
				}
			}
		}
	}
}
/*
{"method":"thing.service.property.set","id":"1767631372","params":{"Lock_control":1},"version":"1.0.0"}
*/

//mqtt��������
void app_task_mqtt(void *pvParameters)
{
	int ret=0,times=0;
	uint8_t nrf_send[32] = {'D','A','T'};
	uint8_t messag[512]={0};
	
	while(1)
	{
		ret = mqtt_connect();
		printf("mqtt_connect %d\n",ret);
		if(ret == 0)
			break;
		vTaskDelay(2000);
		exit_transport();
		vTaskDelay(2000);
	}
	vTaskDelay(500);
	printf("mqtt_subtopic=%d\n",mqtt_subtopic(SUBTOPIC));
	tab2_name_set("Online");
	while(1)
	{
		
		if(RX2_data.a>30)
		{
			for(int i=0;i<RX2_data.a;i++)
			{
				if(memcmp((uint8_t *)&RX2_data.data[i],"Lock_control",12)==0) //������������
				{
					printf("%c\n",RX2_data.data[i+14]);
					if(RX2_data.data[i+14] == '1')
					{
						memset(&nrf_send[3],0,29);
						nrf_send[3]=11;
						nrf_send[4]='O';
						xQueueSend(queue_nrf,nrf_send,100); //����
					}
					break;
				}
			}
			usart2_clear();
		}
		
		memset(&nrf_send[3],0,29);
		nrf_send[3]=10;
		xQueueSend(queue_nrf,nrf_send,100); //��ȡ����״̬
		
		memset(&nrf_send[3],0,29);
		nrf_send[3]=20;
		xQueueSend(queue_nrf,nrf_send,100); //��ȡ����״̬
		
		vTaskDelay(1500);
		sprintf((char *)messag,PUBDATA,temperature,humidity,flame,gas);
		mqtt_publish_data(PUBLIST,(char *)messag,0);
		
		times++;
		if(times>30)
		{
			ret = mqtt_tick();
			printf("tick=%d\n",ret);
			if(ret == 0)
				tab2_name_set("Online");
			else
				tab2_name_set("Offline");
			times = 0;
		}
	}
}
//rtc �������uiʱ��
void app_task_rtc(void *pvParameters)
{
	BaseType_t	xReturn;
	EventBits_t EventValue;
	while(1)
	{
		/* �ȴ��¼����е���Ӧ�¼�λ����ͬ�� */
		EventValue=xEventGroupWaitBits((EventGroupHandle_t	)event_group,		
									   (EventBits_t			)EVENT_GROUP_RTC_WAKEUP,
									   (BaseType_t			)pdTRUE,				
									   (BaseType_t			)pdFALSE,
									   (TickType_t			)portMAX_DELAY);
		
		if(EventValue & EVENT_GROUP_RTC_WAKEUP)
		{ 
			if(get_tab_act() == 0)
			{
				RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure); //��ʱ��Ž�ȫ�ֱ���
				RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure); //�����ڷŽ�ȫ�ֱ���
				update_time();
			}
		}	
	}
}

//music ��������
void app_task_music(void *pvParameters)
{
	EventBits_t EventValue;
	uint32_t i;
	while(1)
	{
		/* �ȴ��¼����е���Ӧ�¼�λ����ͬ�� */
		EventValue=xEventGroupWaitBits((EventGroupHandle_t	)event_group,		
									   (EventBits_t			)EVENT_GROUP_MUSIC,
									   (BaseType_t			)pdTRUE,				
									   (BaseType_t			)pdFALSE,
									   (TickType_t			)portMAX_DELAY);

		vTaskSuspend(app_task_nrf24l01_handler);
		Vs1053_SPI_Init();
		VS_HD_Reset();
		
		VS_Soft_Reset();
		VS_Restart_Play();
		VS_Set_All();        	//������������Ϣ 			 
		VS_Reset_DecodeTime();	//��λ����ʱ�� 	  
		
		fat_res = f_open(&fp, "1:001.mp3",FA_READ);
		printf("f_open res = %d\n", fat_res);
		if(fat_res == 0)
		{
			while(1)
			{
				fat_res=f_read(&fp,databuf,4096, &rw);//����4096���ֽ�  
				i=0; 
				do 
				{
					//Vs1053_SPI_Init();
					if(VS_Send_MusicData(databuf+i)==0)//��VS10XX������Ƶ����
					{
						i+=32;
					}
					//NRF24L01_SPI_Init();
					vTaskDelay(1);
				}while(i<4096);//ѭ������4096���ֽ� 
				
				if(rw!=4096)
					break;
			}
		}
		f_close(&fp);
		NRF24L01_SPI_Init();
		vTaskResume(app_task_nrf24l01_handler);
	}
}


//���Ӻ���
void vApplicationTickHook(void)
{
	lv_tick_inc(1);
}




