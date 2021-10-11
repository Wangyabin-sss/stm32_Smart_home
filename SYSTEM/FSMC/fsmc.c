#include "fsmc.h"

struct	
{
	uint16_t ID;          //������ID	
	uint16_t Width;		  // ˮƽ�ֱ���
	uint16_t Height;		// ��ֱ�ֱ���
	uint8_t  Direction;		//	��ʾ����
}LCD;


void LCD_GPIO_Config(void)
{ 					
 	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��PD,PE,PCʱ��  
	
 	/*--------------------- ---��ʼ��FSMC����-----------------------------------*/ 
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//�������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����

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
	
 	/*-------------------------��ʼ��������������--------------------------------*/ 

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//2MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	
	//��ʼ����λ����
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				
 	GPIO_Init(GPIOC, &GPIO_InitStructure);	    //LCD��λ
	
	//��ʼ����������
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;			 
 	GPIO_Init(GPIOD, &GPIO_InitStructure);		//LCD����
	
	GPIO_SetBits(GPIOD,GPIO_Pin_12);				
}  

/*****************************************************************************************
*	�� �� ��: LCD_FSMC_Config
*	��ڲ���: ��
*	�� �� ֵ: ��
*	��������: ��ʼ��FSMC����
*	˵    ��: ʹ��BANK1
******************************************************************************************/

void LCD_FSMC_Config(void)
{
	FSMC_NORSRAMTimingInitTypeDef  SET;	
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;	
	
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);	//ʹ��FSMCʱ��
	
	SET.FSMC_AddressSetupTime = 0x01;	 		//��ַ����ʱ��
	SET.FSMC_AddressHoldTime = 0x01;		 	//��ַ����ʱ��
	SET.FSMC_DataSetupTime = 0x03;		 		//���ݱ���ʱ��
	SET.FSMC_BusTurnAroundDuration = 0x00;
	SET.FSMC_CLKDivision = 0x00;
	SET.FSMC_DataLatency = 0x00;
	SET.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA 

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

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //��ʼ��FSMC
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  // ʹ��BANK1  
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
*	�� �� ��: LCD_Init
*	��ڲ���: ��
*	�� �� ֵ: ��
*	��������: ��ʼ��LCD������
*	˵    ��: ��Ҫ�ǲ���ILI9488����ؼĴ��������б�Ҫ�����ã�Ĭ��������ʾ
******************************************************************************************/

void LCD_Init(void)
{
	LCD_GPIO_Config();	// ��ʼ��GPIO
	LCD_FSMC_Config();	// ��ʼ��FSMC����
	
  // ��λILI9488
	LCD_RST_L;	delay_ms(10); 	//��ʱ����̫С
	LCD_RST_H;	delay_ms(10); 

	//��ȡLCD��������ID��
	LCD_WR_REG(0XD3);	
	LCD.ID=LCD_RD_DATA();     //�ٶ�
	LCD.ID=LCD_RD_DATA();     //��0x00
	LCD.ID=LCD_RD_DATA();     //��0x94
	LCD.ID<<=8;               //0x94�����8λ
	LCD.ID|=LCD_RD_DATA();    //��0x88����ȡ����ID��
	printf("LCD ID:ILI%x\r\n",LCD.ID);
	if(LCD.ID!=0x9488)while(1);	
	
   // ��ʼ��ILI9341
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
	
	LCD_DisplayMode(Mode_V);    // ������ʾ��������
	LCD_Clear(RED);		    // ����
}


void LCD_DisplayMode(uint8_t direction)
{
	LCD.Direction = direction;		// �޸�ȫ�ֱ����������־λ
	
	if (LCD.Direction == Mode_H) 		// ������ʾ
	{
		LCD_WR_REG(0X36);		// ɨ�跽�����üĴ���
		LCD_WR_DATA(0X28); 	//	ɨ�跽�򣬴��ϵ���	�����ҵ���
		
		LCD_WR_REG(0x2A); 	
		LCD_WR_DATA(0x00);	// ����x���꣨������û�����ã�ֻ����Ҫ��д����ѣ�
		LCD_WR_DATA(0x00); 			
		LCD_WR_DATA(0x01);	// �������x����Ϊ320
		LCD_WR_DATA(0XDF); 
			
		LCD_WR_REG(0x2B); 
		LCD_WR_DATA(0x00);	// ����y���꣨������û�����ã�ֻ����Ҫ��д����ѣ�
		LCD_WR_DATA(0x00); 		
		LCD_WR_DATA(0x01);	// �������y����Ϊ240
		LCD_WR_DATA(0x3F); 		
		
		LCD.Width  = 480;		// ˮƽ���320
		LCD.Height = 320;		// ��ֱ���240
	}
	else if(LCD.Direction == Mode_V)	// ������ʾ
	{
		LCD_WR_REG(0X36);		// ɨ�跽�����üĴ���
		LCD_WR_DATA(0X48); 	//	ɨ�跽�򣬴����ң����ϵ���	
		
		LCD_WR_REG(0x2A); 	
		LCD_WR_DATA(0x00);	// ����x���꣨������û�����ã�ֻ����Ҫ��д����ѣ�
		LCD_WR_DATA(0x00); 			
		LCD_WR_DATA(0x01);   // �������x����Ϊ240
		LCD_WR_DATA(0x3F); 				
                           
		LCD_WR_REG(0x2B);    
		LCD_WR_DATA(0x00);   // ����y���꣨������û�����ã�ֻ����Ҫ��д����ѣ�
		LCD_WR_DATA(0x00); 			
		LCD_WR_DATA(0x01);   // �������y����Ϊ320
		LCD_WR_DATA(0XDF); 			
		                     
		LCD.Width  = 320;    // ˮƽ���240
		LCD.Height = 480;		// ��ֱ���320
	}
}

/*****************************************************************************************
*	�� �� ��:	 LCD_Set_Window
*	��ڲ���:	 sx - ˮƽ����
*			  	 sy - ��ֱ����
				 width  - ������ -1
				 height - �߶ȸ߶� -1
*	�� �� ֵ: ��
*	��������: ������Ļ����д��ķ�Χ
*	˵    ��: 
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
*	�� �� ��:	 LCD_SetCursor
*	��ڲ���:	 x - ˮƽ����
*			  	 y - ��ֱ����
*	�� �� ֵ: ��
*	��������: ������ĻҪ��ʾ���ݵ�����
*	˵    ��: 0x2A Ϊˮƽ����Ĵ�����ַ��0x2BΪ��ֱ����Ĵ�����ַ
******************************************************************************************/

void LCD_SetCursor(uint16_t x, uint16_t y)
{	   
   //ILI9488��������ʱ�����END����Ҳд��ȥ����ΪEND����ͨ����ȷ��������ֱ��д�������
	LCD_WR_REG(0x2A); 							
	LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF); 	//����ˮƽ���꣬��λ��ǰ	
	LCD_WR_DATA(0x01);   // �������X����Ϊ480
	LCD_WR_DATA(0xDF); 	
	LCD_WR_REG(0x2B); 						
	LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF); 	//���ô�ֱ���꣬��λ��ǰ	
	LCD_WR_DATA(0x01);   // �������Y����Ϊ480
	LCD_WR_DATA(0xDF); 
}

void LCD_Clear(uint16_t color)
{ 
	uint32_t i = 0;
	
	LCD_SetCursor(0,0);	   // ������ʼ����
	LCD_WR_REG(0X2C);		// ��ʼд��GRAM

	for(i=0;i<320*480;i++)	// �ܹ���Ҫд��320*240���㣬ILI9341�������Ļ�ķֱ����Զ�������ʾ
	{
		*(__IO uint16_t*) FSMC_DATA = color;	// д�뱳��ɫ
	}
}

/*****************************************************************************************
*	�� �� ��: LCD_RD_OnePoint
*	��ڲ���: Ҫ��ȡ�������
*	�� �� ֵ: ��ȡ���������ֵ,������Χ����0
*	��������: ��ȡһ�����ص��RGBֵ(RGB565��ʽ)
*	˵    ��: ���Ͷ�GRAMָ����Ƚ���һ�μٶ���֮��ILI9488�᷵������16λ��ֵ��ǰһ����RG��ֵ��
             ��8λ������һ����B��ֵ��ռ��8λ��
******************************************************************************************/
uint16_t LCD_RD_OnePoint(uint16_t x,uint16_t y)
{
	uint16_t R_Value = 0, G_Value = 0, B_Value = 0, RGB_Value = 0;

	if(x >= LCD.Width||y >= LCD.Height)return 0;

	LCD_SetCursor(x,y);                       //Ҫ��ȡ�������
	LCD_WR_REG(0X2E);                         //��GRAMָ��
	R_Value = LCD_RD_DATA();                  //�ٶ�
	R_Value = LCD_RD_DATA();                  //��ȡRG��ֵ
	G_Value = (R_Value&0X00FC)<<3;            //�õ�G��ֵ
	R_Value = R_Value&0XF800;                 //�õ�R��ֵ
	B_Value = (LCD_RD_DATA()&0XF800)>>11;     //��ȡ���õ�B��ֵ

	RGB_Value = R_Value | G_Value | B_Value;  //�ϳ�������RGBֵ
	return RGB_Value;
}





