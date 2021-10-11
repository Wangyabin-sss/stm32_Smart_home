#ifndef _XPT2046_H_
#define _XPT2046_H_

#include "Sys.h"

#define T_MOSI  PAout(7)
#define T_MISO  PAin(6)
#define T_CS    PBout(6)
#define T_CLK   PAout(5)

#define  Tcmd_X   0xd0
#define  Tcmd_Y   0x90

void xpt2046_init(void);
void Touch_WriteByte(uint8_t num);
uint16_t Touch_ReadAD(uint8_t cmd);
void touch_getxy(int16_t *posx,int16_t *posy);
int32_t is_Touchdown(void);

#endif 
