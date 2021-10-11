## 2021.9.28 �������

```c
FATFS fsObject;
FIL fp;
UINT rw;
FRESULT res;

uint8_t databuf[4096];
const char wData[12] = {0};

void ov2640_speed_ctrl(void)
{
	uint8_t clkdiv,pclkdiv;			//ʱ�ӷ�Ƶϵ����PCLK��Ƶϵ��
		clkdiv=2;
		pclkdiv=8;
	SCCB_WR_Reg(0XFF,0X00);		
	SCCB_WR_Reg(0XD3,pclkdiv);	//����PCLK��Ƶ
	SCCB_WR_Reg(0XFF,0X01);
	SCCB_WR_Reg(0X11,clkdiv);	//����CLK��Ƶ	
}


int main(void)
{
	uint16_t tmp;
	volatile uint32_t i=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	
	usart1_init(115200);
	LCD_Init();
	rtc_init();
//	lv_init();
//	lv_port_disp_init();   // ��ʾ����ʼ��

//	printf("ov2640_init :%d\n",OV2640_Init());
//	OV2640_RGB565_Mode();
//	printf("OV2640_OutSize_Set :%d\n",OV2640_OutSize_Set(320,480)); 
//	ov2640_speed_ctrl();
//	OV2640_Auto_Exposure(1);
//	OV2640_Color_Saturation(2);
//	OV2640_Contrast(3);
	
	while(SD_Init())//��ⲻ��SD��
	{
		delay_ms(500);
		printf("����sd��\n");
	}
	
	res = f_mount(&fsObject, "1:", 1);	//����sd��
	printf("f_mount res = %d\n", res);
	if(res!=0)
		while(1);
	
/*
	res = f_open(&fp, "1:abc.txt",FA_READ);
	printf("f_open res = %d\n", res);
	
	if (res == FR_OK)
	{
		res = f_read(&fp, (void *)wData, sizeof(wData), &rw);
		printf("�ɹ�����bw = %d\n", rw);
	}
	printf("%s\n",wData);
	
	f_close(&fp);
*/

	VS_Init();	 //��ʼ��VS1053 
	//VS_SPK_Set(0);
	
	printf("Ram Test:0X%04X\r\n",VS_Ram_Test());//��ӡRAM���Խ��	
	
	
	while(1)
	{
//		lv_tick_inc(5);
//		lv_task_handler();
//		delay_ms(5);
		/*
		VS_Soft_Reset();
		VS_Sine_Test();	 
		
		VS_Soft_Reset();
		VS_Restart_Play();  					//�������� 
		VS_Set_All();        					//������������Ϣ 			 
		VS_Reset_DecodeTime();					//��λ����ʱ�� 	  
		
		res = f_open(&fp, "1:�����ҵȴ�.mp3",FA_READ);
		printf("f_open res = %d\n", res);
		if(res == 0)
		{
			VS_SPI_SpeedHigh();	//����		
			while(1)
			{
				res=f_read(&fp,databuf,4096, &rw);//����4096���ֽ�  
				i=0; 
				do 
				{
					if(VS_Send_MusicData(databuf+i)==0)//��VS10XX������Ƶ����
					{
						i+=32;
					}
				}while(i<4096);//ѭ������4096���ֽ� 
				
				if(rw!=4096)
					break;
			}
		}
		f_close(&fp);
		
		
		delay_ms(1000);
		*/
		
		if(alarm_ready==1){
			alarm_ready=0;
			RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure);
			printf("%02x:%02x:%02x\r\n",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);

			//��ȡ����
			RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);
			printf("20%02x/%02x/%02xWeek:%x\r\n",RTC_DateStructure.RTC_Year,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date,RTC_DateStructure.RTC_WeekDay);
		}
		
//		while(OV2640_VSYNC==1)	//��ʼ�ɼ�����
//		{
//			while(OV2640_HREF)
//			{ 
//				while(OV2640_PCLK==0);
//				i*=10;
//				tmp = (uint16_t)OV2640_DATA;
//				while(OV2640_PCLK==1);  
//				while(OV2640_PCLK==0); 
//				i*=10;
//				*(__IO uint16_t*) FSMC_DATA = (uint16_t)(((uint16_t)OV2640_DATA<<8)|tmp); 
//				while(OV2640_PCLK==1);
//			} 
//		}

	}
}

```