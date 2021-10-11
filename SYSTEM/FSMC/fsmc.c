#include "fsmc.h"

struct	
{
	uint16_t ID;          //控制器ID	
	uint16_t Width;		  // 水平分辨率
	uint16_t Height;		// 垂直分辨率
	uint8_t  Direction;		//	显示方向
}LCD;


void LCD_GPIO_Config(void)
{ 					
 	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOC, ENABLE);//使能PD,PE,PC时钟  
	
 	/*--------------------- ---初始化FSMC引脚-----------------------------------*/ 
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1  | GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_7  | GPIO_Pin_8 | 
								  GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11 |
								  GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);  //FSMC_D0-FSMC_D15
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);  
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC); 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);  //FSMC_NOE
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);  //FSMC_NWE
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource11,GPIO_AF_FSMC); //FSMC_A16
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource7,GPIO_AF_FSMC);  //FSMC_NE1
	
 	/*-------------------------初始化其他控制引脚--------------------------------*/ 

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//2MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	
	//初始化复位引脚
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				
 	GPIO_Init(GPIOC, &GPIO_InitStructure);	    //LCD复位
	
	//初始化背光引脚
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;			 
 	GPIO_Init(GPIOD, &GPIO_InitStructure);		//LCD背光
	
	GPIO_SetBits(GPIOD,GPIO_Pin_12);				
}  

/*****************************************************************************************
*	函 数 名: LCD_FSMC_Config
*	入口参数: 无
*	返 回 值: 无
*	函数功能: 初始化FSMC配置
*	说    明: 使用BANK1
******************************************************************************************/

void LCD_FSMC_Config(void)
{
	FSMC_NORSRAMTimingInitTypeDef  SET;	
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;	
	
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);	//使能FSMC时钟
	
	SET.FSMC_AddressSetupTime = 0x01;	 		//地址建立时间
	SET.FSMC_AddressHoldTime = 0x01;		 	//地址保持时间
	SET.FSMC_DataSetupTime = 0x03;		 		//数据保存时间
	SET.FSMC_BusTurnAroundDuration = 0x00;
	SET.FSMC_CLKDivision = 0x00;
	SET.FSMC_DataLatency = 0x00;
	SET.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 

	FSMC_NORSRAMInitStructure.FSMC_Bank 		      = FSMC_Bank1_NORSRAM1;			
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux 	  = FSMC_DataAddressMux_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_MemoryType 		  = FSMC_MemoryType_SRAM;			
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth 	  = FSMC_MemoryDataWidth_16b;		
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode 	  = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait	  = FSMC_AsynchronousWait_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_WrapMode 			  = FSMC_WrapMode_Disable;   
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive   = FSMC_WaitSignalActive_BeforeWaitState;  
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation 	  = FSMC_WriteOperation_Enable;					
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal 		  = FSMC_WaitSignal_Disable;   
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode		  = FSMC_ExtendedMode_Enable; 	
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst 		  = FSMC_WriteBurst_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct  = &SET;  

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  // 使能BANK1  
}


void LCD_WR_REG(uint16_t reg)
{   
	*(__IO uint16_t*) FSMC_REG = reg;	 
}

void LCD_WR_DATA(uint16_t data)
{	 
	*(__IO uint16_t*) FSMC_DATA = data;	
}
	
uint16_t LCD_RD_DATA(void)
{
    uint16_t data;	
	data = *(__IO uint16_t*) FSMC_DATA;	
	return data;	 
}

/*****************************************************************************************
*	函 数 名: LCD_Init
*	入口参数: 无
*	返 回 值: 无
*	函数功能: 初始化LCD的配置
*	说    明: 主要是操作ILI9488的相关寄存器，进行必要的设置，默认竖屏显示
******************************************************************************************/

void LCD_Init(void)
{
	LCD_GPIO_Config();	// 初始化GPIO
	LCD_FSMC_Config();	// 初始化FSMC配置
	
  // 复位ILI9488
	LCD_RST_L;	delay_ms(10); 	//延时不能太小
	LCD_RST_H;	delay_ms(10); 

	//读取LCD控制器的ID号
	LCD_WR_REG(0XD3);	
	LCD.ID=LCD_RD_DATA();     //假读
	LCD.ID=LCD_RD_DATA();     //读0x00
	LCD.ID=LCD_RD_DATA();     //读0x94
	LCD.ID<<=8;               //0x94移入高8位
	LCD.ID|=LCD_RD_DATA();    //读0x88并获取完整ID号
	printf("LCD ID:ILI%x\r\n",LCD.ID);
	if(LCD.ID!=0x9488)while(1);	
	
   // 初始化ILI9341
	LCD_WR_REG(0xE0); //P-Gamma
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x17);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x3E);
	LCD_WR_DATA(0x89);
	LCD_WR_DATA(0x49);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0x0D);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x13);
	LCD_WR_DATA(0x15);
	LCD_WR_DATA(0x0F);
	LCD_WR_REG(0XE1); //N-Gamma
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x15);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x05);
	LCD_WR_DATA(0x2D);
	LCD_WR_DATA(0x34);
	LCD_WR_DATA(0x41);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x0B);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x33);
	LCD_WR_DATA(0x37);
	LCD_WR_DATA(0x0F);

	LCD_WR_REG(0XC0); //Power Control 1
	LCD_WR_DATA(0x17); //Vreg1out
	LCD_WR_DATA(0x15); //Verg2out
	LCD_WR_REG(0xC1); //Power Control 2
	LCD_WR_DATA(0x41); //VGH,VGL
	LCD_WR_REG(0xC5); //Power Control 3
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x12); //Vcom
	LCD_WR_DATA(0x80);

	LCD_WR_REG(0x36); //Memory Access
	LCD_WR_DATA(0x48);
	LCD_WR_REG(0x3A); // Interface Pixel Format
	LCD_WR_DATA(0x55);
	LCD_WR_REG(0XB0); // Interface Mode Control
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0xB1); //Frame rate
	LCD_WR_DATA(0xA0); //60Hz
	LCD_WR_REG(0xB4); //Display Inversion Control
	LCD_WR_DATA(0x02); //2-dot
	LCD_WR_REG(0XB6); //RGB/MCU Interface Control
	LCD_WR_DATA(0x02); //MCU
	LCD_WR_DATA(0x02); //Source,Gate scan dieection
	LCD_WR_REG(0XE9); // Set Image Function
	LCD_WR_DATA(0x00); // Disable 24 bit data input
	LCD_WR_REG(0xF7);// Adjust Control
	LCD_WR_DATA(0xA9);
	LCD_WR_DATA(0x51);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x82);
	// D7 stream, loose
	LCD_WR_REG(0x11); //Sleep out
	delay_ms(20);
	LCD_WR_REG(0x29); //Display on
	
	LCD_DisplayMode(Mode_V);    // 设置显示方向，竖屏
	LCD_Clear(RED);		    // 清屏
}


void LCD_DisplayMode(uint8_t direction)
{
	LCD.Direction = direction;		// 修改全局变量，方向标志位
	
	if (LCD.Direction == Mode_H) 		// 横屏显示
	{
		LCD_WR_REG(0X36);		// 扫描方向设置寄存器
		LCD_WR_DATA(0X28); 	//	扫描方向，从上到下	，从右到左
		
		LCD_WR_REG(0x2A); 	
		LCD_WR_DATA(0x00);	// 设置x坐标（在这里没有作用，只是需要先写入而已）
		LCD_WR_DATA(0x00); 			
		LCD_WR_DATA(0x01);	// 设置最大x坐标为320
		LCD_WR_DATA(0XDF); 
			
		LCD_WR_REG(0x2B); 
		LCD_WR_DATA(0x00);	// 设置y坐标（在这里没有作用，只是需要先写入而已）
		LCD_WR_DATA(0x00); 		
		LCD_WR_DATA(0x01);	// 设置最大y坐标为240
		LCD_WR_DATA(0x3F); 		
		
		LCD.Width  = 480;		// 水平宽度320
		LCD.Height = 320;		// 垂直宽度240
	}
	else if(LCD.Direction == Mode_V)	// 竖屏显示
	{
		LCD_WR_REG(0X36);		// 扫描方向设置寄存器
		LCD_WR_DATA(0X48); 	//	扫描方向，从左到右，从上到下	
		
		LCD_WR_REG(0x2A); 	
		LCD_WR_DATA(0x00);	// 设置x坐标（在这里没有作用，只是需要先写入而已）
		LCD_WR_DATA(0x00); 			
		LCD_WR_DATA(0x01);   // 设置最大x坐标为240
		LCD_WR_DATA(0x3F); 				
                           
		LCD_WR_REG(0x2B);    
		LCD_WR_DATA(0x00);   // 设置y坐标（在这里没有作用，只是需要先写入而已）
		LCD_WR_DATA(0x00); 			
		LCD_WR_DATA(0x01);   // 设置最大y坐标为320
		LCD_WR_DATA(0XDF); 			
		                     
		LCD.Width  = 320;    // 水平宽度240
		LCD.Height = 480;		// 垂直宽度320
	}
}

/*****************************************************************************************
*	函 数 名:	 LCD_Set_Window
*	入口参数:	 sx - 水平坐标
*			  	 sy - 垂直坐标
				 width  - 所需宽度 -1
				 height - 高度高度 -1
*	返 回 值: 无
*	函数功能: 设置屏幕可以写入的范围
*	说    明: 
******************************************************************************************/
void LCD_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{ 
	uint16_t twidth,theight;
	twidth=sx+width;
	theight=sy+height;
	
	LCD_WR_REG(0x2A);
    LCD_WR_DATA(sx>>8);
    LCD_WR_DATA(sx&0XFF);
    LCD_WR_DATA(twidth>>8);
    LCD_WR_DATA(twidth&0XFF);
    LCD_WR_REG(0x2B);
    LCD_WR_DATA(sy>>8);
    LCD_WR_DATA(sy&0XFF);
    LCD_WR_DATA(theight>>8);
    LCD_WR_DATA(theight&0XFF);
	
	LCD_WR_REG(0X2C);
}


/*****************************************************************************************
*	函 数 名:	 LCD_SetCursor
*	入口参数:	 x - 水平坐标
*			  	 y - 垂直坐标
*	返 回 值: 无
*	函数功能: 设置屏幕要显示数据的坐标
*	说    明: 0x2A 为水平坐标寄存器地址，0x2B为垂直坐标寄存器地址
******************************************************************************************/

void LCD_SetCursor(uint16_t x, uint16_t y)
{	   
   //ILI9488设置坐标时必须把END坐标也写进去。因为END坐标通常不确定，这里直接写最大坐标
	LCD_WR_REG(0x2A); 							
	LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF); 	//设置水平坐标，高位在前	
	LCD_WR_DATA(0x01);   // 设置最大X坐标为480
	LCD_WR_DATA(0xDF); 	
	LCD_WR_REG(0x2B); 						
	LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF); 	//设置垂直坐标，高位在前	
	LCD_WR_DATA(0x01);   // 设置最大Y坐标为480
	LCD_WR_DATA(0xDF); 
}

void LCD_Clear(uint16_t color)
{ 
	uint32_t i = 0;
	
	LCD_SetCursor(0,0);	   // 设置起始坐标
	LCD_WR_REG(0X2C);		// 开始写入GRAM

	for(i=0;i<320*480;i++)	// 总共需要写入320*240个点，ILI9341会根据屏幕的分辨率自动换行显示
	{
		*(__IO uint16_t*) FSMC_DATA = color;	// 写入背景色
	}
}

/*****************************************************************************************
*	函 数 名: LCD_RD_OnePoint
*	入口参数: 要读取点的坐标
*	返 回 值: 读取到点的坐标值,超出范围返回0
*	函数功能: 读取一个像素点的RGB值(RGB565格式)
*	说    明: 发送读GRAM指令后，先进行一次假读，之后ILI9488会返回两个16位的值，前一个是RG的值，
             各8位，后面一个是B的值，占高8位。
******************************************************************************************/
uint16_t LCD_RD_OnePoint(uint16_t x,uint16_t y)
{
	uint16_t R_Value = 0, G_Value = 0, B_Value = 0, RGB_Value = 0;

	if(x >= LCD.Width||y >= LCD.Height)return 0;

	LCD_SetCursor(x,y);                       //要读取点的坐标
	LCD_WR_REG(0X2E);                         //读GRAM指令
	R_Value = LCD_RD_DATA();                  //假读
	R_Value = LCD_RD_DATA();                  //获取RG的值
	G_Value = (R_Value&0X00FC)<<3;            //得到G的值
	R_Value = R_Value&0XF800;                 //得到R的值
	B_Value = (LCD_RD_DATA()&0XF800)>>11;     //获取并得到B的值

	RGB_Value = R_Value | G_Value | B_Value;  //合成完整的RGB值
	return RGB_Value;
}





