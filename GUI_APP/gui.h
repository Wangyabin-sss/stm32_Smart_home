#ifndef _GUI_APP_H
#define _GUI_APP_H

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

void lv_gui_init(void);
void tab2_name_set(char *name);
void tab2_dev_add(char num,int mode);
uint16_t get_tab_act(void);		//获取当前活动标签
void update_time(void);		//时间日期更新
void updata_thfg(float T,float H,uint32_t F,uint32_t G);

#endif
