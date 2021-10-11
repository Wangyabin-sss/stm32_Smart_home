#ifndef __SCCB_H
#define __SCCB_H

#include "Sys.h"

//SCCB 驱动代码	

#define SCCB_SDA_IN()  {GPIOE->MODER&=~(3<<(6*2));GPIOE->MODER|=0<<6*2;}	//PE6输入模式
#define SCCB_SDA_OUT() {GPIOE->MODER&=~(3<<(6*2));GPIOE->MODER|=1<<6*2;}    //PE6输出模式

//IO操作函数	 
#define SCCB_SCL		PEout(5)

#define SCCB_SDA		PEout(6)
#define SCCB_READ_SDA   PEin(6)

#define SCCB_ID   		 0X60  			//OV2640的ID



///////////////////////////////////////////
void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
uint8_t SCCB_WR_Byte(uint8_t dat);
uint8_t SCCB_RD_Byte(void);
uint8_t SCCB_WR_Reg(uint8_t reg,uint8_t data);
uint8_t SCCB_RD_Reg(uint8_t reg);



#endif







