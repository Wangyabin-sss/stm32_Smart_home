## 2021.9.28 裸机测试

```c
FATFS fsObject;
FIL fp;
UINT rw;
FRESULT res;

uint8_t databuf[4096];
const char wData[12] = {0};

void ov2640_speed_ctrl(void)
{
	uint8_t clkdiv,pclkdiv;			//时钟分频系数和PCLK分频系数
		clkdiv=2;
		pclkdiv=8;
	SCCB_WR_Reg(0XFF,0X00);		
	SCCB_WR_Reg(0XD3,pclkdiv);	//设置PCLK分频
	SCCB_WR_Reg(0XFF,0X01);
	SCCB_WR_Reg(0X11,clkdiv);	//设置CLK分频	
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
//	lv_port_disp_init();   // 显示器初始化

//	printf("ov2640_init :%d\n",OV2640_Init());
//	OV2640_RGB565_Mode();
//	printf("OV2640_OutSize_Set :%d\n",OV2640_OutSize_Set(320,480)); 
//	ov2640_speed_ctrl();
//	OV2640_Auto_Exposure(1);
//	OV2640_Color_Saturation(2);
//	OV2640_Contrast(3);
	
	while(SD_Init())//检测不到SD卡
	{
		delay_ms(500);
		printf("请检查sd卡\n");
	}
	
	res = f_mount(&fsObject, "1:", 1);	//挂载sd卡
	printf("f_mount res = %d\n", res);
	if(res!=0)
		while(1);
	
/*
	res = f_open(&fp, "1:abc.txt",FA_READ);
	printf("f_open res = %d\n", res);
	
	if (res == FR_OK)
	{
		res = f_read(&fp, (void *)wData, sizeof(wData), &rw);
		printf("成功读出bw = %d\n", rw);
	}
	printf("%s\n",wData);
	
	f_close(&fp);
*/

	VS_Init();	 //初始化VS1053 
	//VS_SPK_Set(0);
	
	printf("Ram Test:0X%04X\r\n",VS_Ram_Test());//打印RAM测试结果	
	
	
	while(1)
	{
//		lv_tick_inc(5);
//		lv_task_handler();
//		delay_ms(5);
		/*
		VS_Soft_Reset();
		VS_Sine_Test();	 
		
		VS_Soft_Reset();
		VS_Restart_Play();  					//重启播放 
		VS_Set_All();        					//设置音量等信息 			 
		VS_Reset_DecodeTime();					//复位解码时间 	  
		
		res = f_open(&fp, "1:别让我等待.mp3",FA_READ);
		printf("f_open res = %d\n", res);
		if(res == 0)
		{
			VS_SPI_SpeedHigh();	//高速		
			while(1)
			{
				res=f_read(&fp,databuf,4096, &rw);//读出4096个字节  
				i=0; 
				do 
				{
					if(VS_Send_MusicData(databuf+i)==0)//给VS10XX发送音频数据
					{
						i+=32;
					}
				}while(i<4096);//循环发送4096个字节 
				
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

			//获取日期
			RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);
			printf("20%02x/%02x/%02xWeek:%x\r\n",RTC_DateStructure.RTC_Year,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date,RTC_DateStructure.RTC_WeekDay);
		}
		
//		while(OV2640_VSYNC==1)	//开始采集数据
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