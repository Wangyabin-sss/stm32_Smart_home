#ifndef _GUI_APP_H
#define _GUI_APP_H

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

void lv_gui_init(void);
void tab2_name_set(char *name);
void tab2_dev_add(char num,int mode);
uint16_t get_tab_act(void);		//��ȡ��ǰ���ǩ
void update_time(void);		//ʱ�����ڸ���
void updata_thfg(float T,float H,uint32_t F,uint32_t G);

#endif
