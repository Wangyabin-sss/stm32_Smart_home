#ifndef _FSMC_H_
#define _FSMC_H_

#include "Sys.h"
#include "usart.h"



#define  WHITE         	 0xFFFF
#define  BLACK         	 0x0000
#define  BLUE         	 0x001F
#define  RED           	 0xF800
#define  GREEN         	 0x07E0
#define  YELLOW        	 0xFFE0
#define  BROWN 			 0XBC40 //棕色
#define  BRRED 			 0XFC07 //棕红色
#define  GRAY  			 0X8430 //灰色

#define	 Mode_H		 0		//LCD水平显示
#define	 Mode_V		 1		//LCD竖屏显示

#define 	FSMC_REG     0x60000000		// FSMC 写寄存器地址
#define 	FSMC_DATA    0x60020000		// FSMC 写数据地址

#define LCD_RST_L  PCout(2)=0
#define LCD_RST_H  PCout(2)=1


void LCD_WR_DATA(uint16_t data);
void LCD_Init(void);
void LCD_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);
void LCD_SetCursor(uint16_t x, uint16_t y);
void LCD_DisplayMode(uint8_t direction);
void LCD_Clear(uint16_t color);
uint16_t LCD_RD_OnePoint(uint16_t x,uint16_t y);

#endif
