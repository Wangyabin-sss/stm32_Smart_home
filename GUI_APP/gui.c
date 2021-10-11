#include "main.h"

#define TAB2USRPAGE		30
#define TAB3USRPAGE		40

static FIL fp;
static FRESULT fat_res;
static uint8_t nrf_send[32]={'D','A','T'};

static char *settime23   ="0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23";
static char *settime59   ="0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59";
static char *setdateyear ="2020\n2021\n2022\n2023\n2024\n2025\n2026\n2027\n2028\n2029";
static char *setdatemonth="1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12";
static char *setdatedata ="1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31";
static char *setdateweek ="1\n2\n3\n4\n5\n6\n7";

static lv_obj_t * tabview;
/***********tab1相关变量******************/
static lv_obj_t * tab1;
static lv_obj_t * calendar;
static lv_calendar_date_t today;
/***********tab2相关变量******************/
static lv_obj_t * tab2;
static lv_obj_t * tab2_list;
static lv_obj_t * tab2_list_btn;
static char *tab2_dev_list[3]={"REFRESH","lock","environment"};//tab2设备列表标签
/***********tab3相关变量******************/
static lv_obj_t * tab3;
static lv_obj_t * tab3_list;
static lv_obj_t * tab3_list_btn;
static char *tab3_set_list[10]={"WIFI","HOME LOCK","TIME SET","DATE SET"};//tab3设置列表标签
/************对话框公用变量********************/
static lv_obj_t * listpage = NULL;
static lv_obj_t * btncancel;
static lv_obj_t * btnok;
static lv_obj_t * btn1;
static uint32_t tab_list_page_now=0;	//记录当前活跃对话框
static lv_obj_t * set_ta1;
static lv_obj_t * set_ta2;
static lv_obj_t *set_roller1;
static lv_obj_t *set_roller2;
static lv_obj_t *set_roller3;
static lv_obj_t *set_roller4;
/************公共键盘*********************/
static lv_obj_t * kb;

static void kb_event_cb(lv_obj_t * keyboard, lv_event_t e)
{
    lv_keyboard_def_event_cb(kb, e);
    if(e == LV_EVENT_CANCEL) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_del(kb);
        kb = NULL;
    }
	else if(e == LV_EVENT_APPLY) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_del(kb);
        kb = NULL;
    }
}

void kb_create(lv_obj_t *textarea)
{
    kb = lv_keyboard_create(lv_scr_act(), NULL);
    lv_keyboard_set_cursor_manage(kb, true);
    lv_obj_set_event_cb(kb, kb_event_cb);
    lv_keyboard_set_textarea(kb, textarea);
}
/******************提示框*********************************/
static void mbox_create(char *msg)
{
	static const char * btns2[] = {"Ok",""};

	lv_obj_t* mbox = lv_msgbox_create(lv_scr_act(), NULL);
    lv_msgbox_set_text(mbox, msg);
    lv_msgbox_add_btns(mbox, btns2);
    lv_msgbox_stop_auto_close(mbox);
	lv_msgbox_set_anim_time(mbox, 100);
    lv_obj_set_pos(mbox, 0, 200);
	
}
/******************对话框创建&回调************************/

static void ta_event_cb(lv_obj_t * ta_local, lv_event_t e)     
{
	if(e == LV_EVENT_CLICKED && kb == NULL) {
		kb_create(ta_local);
	}
}

static void listpage_btn1_handler(lv_obj_t * obj, lv_event_t event)
{
	if(event == LV_EVENT_CLICKED)
	{
		memset(&nrf_send[3],0,29);
		nrf_send[3]=11;
		nrf_send[4]='O';
		xQueueSend(queue_nrf,nrf_send,100); //设置密码
	}
}

static void listpage_btn_handler(lv_obj_t * obj, lv_event_t event)      //列表对话框确认取消键回调
{
    if(event == LV_EVENT_CLICKED) {
        if(obj==btncancel)       //cancel键
		{
			lv_obj_del(listpage);
			listpage=NULL;
			tab_list_page_now=0;
		}
		else if(obj==btnok) 	//OK键
		{
			switch(tab_list_page_now)
			{
				case TAB2USRPAGE+1: //刷新
					break;
				case TAB2USRPAGE+2: //lock
					
					break;
				case TAB2USRPAGE+3: //environment
					break;
				case TAB3USRPAGE+1:		//wifi设置
				{
					const char *name=lv_textarea_get_text(set_ta1);
					const char *pwd =lv_textarea_get_text(set_ta2);
					fat_res = f_open(&fp, "1:wifi.txt",FA_WRITE | FA_OPEN_ALWAYS);
					if(fat_res == 0)
					{
						f_puts(name,&fp);
						f_puts("\r\n",&fp);
						f_puts(pwd,&fp);
						f_puts("\r\n",&fp);
					}
					f_close(&fp);
					mbox_create("Please restart");
					break;
				}
				case TAB3USRPAGE+2:		//门锁密码设置
					memset(&nrf_send[3],0,29);
					nrf_send[3]=11;
					nrf_send[4]='P';
					const char *pwd=lv_textarea_get_text(set_ta1);
					printf("密码%s\n",pwd);
					memcpy(&nrf_send[5],pwd,6);
					xQueueSend(queue_nrf,nrf_send,100); //设置密码
					break;
				case TAB3USRPAGE+3:		//时间设置
					RTC_TimeStructure.RTC_Hours   = DEC2BCD(lv_roller_get_selected(set_roller1));
					RTC_TimeStructure.RTC_Minutes = DEC2BCD(lv_roller_get_selected(set_roller2));
					RTC_TimeStructure.RTC_Seconds = DEC2BCD(lv_roller_get_selected(set_roller3));
					RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
					break;
				case TAB3USRPAGE+4:		//日期设置
					RTC_DateStructure.RTC_Year    = DEC2BCD(lv_roller_get_selected(set_roller1)+20);
					RTC_DateStructure.RTC_Month   = DEC2BCD(lv_roller_get_selected(set_roller2)+1);
					RTC_DateStructure.RTC_Date    = DEC2BCD(lv_roller_get_selected(set_roller3)+1);
					RTC_DateStructure.RTC_WeekDay = DEC2BCD(lv_roller_get_selected(set_roller4)+1);
					RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
					break;
				default:
					break;
			}
			lv_obj_del(listpage);
			listpage=NULL;
			tab_list_page_now=0;
		}	
    }
}

static void lv_list_page_create(lv_obj_t *tab,uint8_t setnum)            //列表对话框创建
{
	if(listpage==NULL)
	{
		lv_obj_t *label;
		tab_list_page_now=setnum;                    //设置当前是哪个list的哪个btn
		listpage=lv_obj_create(tab, NULL);
		lv_obj_set_size(listpage, 300, 240);
		lv_obj_align(listpage, NULL, LV_ALIGN_CENTER, 0 ,-60);
		btncancel=lv_btn_create(listpage,NULL);        //取消按钮
		lv_obj_set_size(btncancel, 80, 40);
		lv_obj_align(btncancel, NULL, LV_ALIGN_CENTER, -100 ,80);
		label=lv_label_create(btncancel,NULL);
		lv_label_set_text(label,"Cancel");
		lv_obj_set_event_cb(btncancel, listpage_btn_handler);
		btnok=lv_btn_create(listpage,NULL);        //确认按钮
		lv_obj_set_size(btnok, 80, 40);
		lv_obj_align(btnok, NULL, LV_ALIGN_CENTER, 100 ,80);
		label=lv_label_create(btnok,NULL);
		lv_label_set_text(label,"Ok");
		lv_obj_set_event_cb(btnok, listpage_btn_handler);
		switch(setnum)
		{
			case TAB2USRPAGE+1:
				break;
			case TAB2USRPAGE+2: //lock
				btn1=lv_btn_create(listpage,NULL);        //确认按钮
				lv_obj_set_size(btn1, 80, 40);
				lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0 ,0);
				label = lv_label_create(btn1,NULL);
				lv_label_set_text(label,"open");
				lv_obj_set_event_cb(btn1, listpage_btn1_handler);
				break;
			case TAB2USRPAGE+3: //environment
				btn1=lv_btn_create(listpage,NULL);        //确认按钮
				lv_obj_set_size(btn1, 280, 110);
				lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0 ,-20);
				break;
			case TAB3USRPAGE+1:		//wifi设置对话框
				set_ta1=lv_textarea_create(listpage,NULL);
				lv_textarea_set_text(set_ta1,	(char *)WIFI_NAME);
				lv_textarea_set_cursor_hidden(set_ta1, true);
				lv_obj_set_width(set_ta1, 260);
				lv_textarea_set_one_line(set_ta1, true);
				lv_textarea_set_max_length(set_ta1, 16);
				lv_obj_align(set_ta1, NULL, LV_ALIGN_CENTER, 0 ,-30);
				lv_obj_set_event_cb(set_ta1, ta_event_cb);
				set_ta2=lv_textarea_create(listpage,NULL);
				lv_textarea_set_text(set_ta2, (char *)WIFI_PWD);
				lv_textarea_set_cursor_hidden(set_ta2, true);
				lv_obj_set_width(set_ta2, 260);
				lv_textarea_set_one_line(set_ta2, true);
				lv_textarea_set_max_length(set_ta2, 16);
				lv_obj_align(set_ta2, NULL, LV_ALIGN_CENTER, 0 ,10);
				lv_obj_set_event_cb(set_ta2, ta_event_cb);
				break;
			case TAB3USRPAGE+2:		//门锁管理员密码对话框
				set_ta1=lv_textarea_create(listpage,NULL);
				lv_textarea_set_text(set_ta1, "000000");
				lv_textarea_set_cursor_hidden(set_ta1, true);
				lv_obj_set_width(set_ta1, 260);
				lv_textarea_set_one_line(set_ta1, true);
				lv_textarea_set_accepted_chars(set_ta1, "0123456789");
				lv_textarea_set_max_length(set_ta1, 6);
				lv_obj_align(set_ta1, NULL, LV_ALIGN_CENTER, 0 ,-20);
				lv_obj_set_event_cb(set_ta1, ta_event_cb);
				break;
			case TAB3USRPAGE+3:		//时间设置对话框
				set_roller1 = lv_roller_create(listpage, NULL);
				lv_roller_set_options(set_roller1,settime23,LV_ROLLER_MODE_INFINITE);
				lv_roller_set_visible_row_count(set_roller1, 3);
				lv_obj_align(set_roller1, NULL, LV_ALIGN_CENTER, -60, -20);
				set_roller2 = lv_roller_create(listpage, NULL);
				lv_roller_set_options(set_roller2,settime59,LV_ROLLER_MODE_INFINITE);
				lv_roller_set_visible_row_count(set_roller2, 3);
				lv_obj_align(set_roller2, NULL, LV_ALIGN_CENTER,  0,  -20);
				set_roller3 = lv_roller_create(listpage, NULL);
				lv_roller_set_options(set_roller3,settime59,LV_ROLLER_MODE_INFINITE);
				lv_roller_set_visible_row_count(set_roller3, 3);
				lv_obj_align(set_roller3, NULL, LV_ALIGN_CENTER, 60,  -20);
				break;
			case TAB3USRPAGE+4:		//日期设置对话框
				set_roller1 = lv_roller_create(listpage, NULL);
				lv_roller_set_options(set_roller1,setdateyear,LV_ROLLER_MODE_INFINITE);
				lv_roller_set_visible_row_count(set_roller1, 3);
				lv_obj_align(set_roller1, NULL, LV_ALIGN_CENTER, -80, -20);
				set_roller2 = lv_roller_create(listpage, NULL);
				lv_roller_set_options(set_roller2,setdatemonth,LV_ROLLER_MODE_INFINITE);
				lv_roller_set_visible_row_count(set_roller2, 3);
				lv_obj_align(set_roller2, NULL, LV_ALIGN_CENTER,  -20,  -20);
				set_roller3 = lv_roller_create(listpage, NULL);
				lv_roller_set_options(set_roller3,setdatedata,LV_ROLLER_MODE_INFINITE);
				lv_roller_set_visible_row_count(set_roller3, 3);
				lv_obj_align(set_roller3, NULL, LV_ALIGN_CENTER, 40,  -20);
				set_roller4 = lv_roller_create(listpage, NULL);
				lv_roller_set_options(set_roller4,setdateweek,LV_ROLLER_MODE_INFINITE);
				lv_roller_set_visible_row_count(set_roller4, 3);
				lv_obj_align(set_roller4, NULL, LV_ALIGN_CENTER, 100,  -20);
				break;
			
			default:
				break;
		}
	}
}
/*************************tab2  list  按钮回调**************************/
static void tab2_list_handler(lv_obj_t * obj, lv_event_t event)    
{
	if(event == LV_EVENT_CLICKED) {
		if(strcmp(lv_list_get_btn_text(obj),tab2_dev_list[0])==0)
		{
			tab2_dev_add(0,0); //先清空
			tab2_list_btn = lv_list_add_btn(tab2_list,LV_SYMBOL_REFRESH,tab2_dev_list[0]); 
			lv_obj_set_event_cb(tab2_list_btn, tab2_list_handler);
			device_overload(nrf_send);
		}
		else if(strcmp(lv_list_get_btn_text(obj),tab2_dev_list[1])==0)
		{
			lv_list_page_create(tab2,TAB2USRPAGE+2);
		}
		else if(strcmp(lv_list_get_btn_text(obj),tab2_dev_list[2])==0)
		{
			lv_list_page_create(tab2,TAB2USRPAGE+3);
		}
		
	}
}

/*************************tab3  list  按钮回调*************************/
static void tab3_list_handler(lv_obj_t * obj, lv_event_t event)    
{
	if(event == LV_EVENT_CLICKED) {
        if(strcmp(lv_list_get_btn_text(obj),tab3_set_list[0])==0)
		{
			lv_list_page_create(tab3,TAB3USRPAGE+1);
		}
		else if(strcmp(lv_list_get_btn_text(obj),tab3_set_list[1])==0)
		{
			lv_list_page_create(tab3,TAB3USRPAGE+2);
		}
		else if(strcmp(lv_list_get_btn_text(obj),tab3_set_list[2])==0)
		{
			lv_list_page_create(tab3,TAB3USRPAGE+3);
		}
		else if(strcmp(lv_list_get_btn_text(obj),tab3_set_list[3])==0)
		{
			lv_list_page_create(tab3,TAB3USRPAGE+4);
		}
	}
}

void lv_gui_init(void)
{
	tabview = lv_tabview_create(lv_scr_act(), NULL);
	
	lv_tabview_set_anim_time(tabview, 60);
	
	tab1 = lv_tabview_add_tab(tabview, "Date");
    tab2 = lv_tabview_add_tab(tabview, "Offline");
    tab3 = lv_tabview_add_tab(tabview, "SET");
	/******tab1******/
	calendar = lv_calendar_create(tab1, NULL);	//创建一个日历
	lv_obj_set_size(calendar, 250, 250);
	lv_obj_align(calendar, NULL, LV_ALIGN_CENTER, 0, 0);
	RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);  //获取当前日期
	today.year  = BCD2DEC(RTC_DateStructure.RTC_Year)+2000;
	today.month = BCD2DEC(RTC_DateStructure.RTC_Month);
	today.day   = BCD2DEC(RTC_DateStructure.RTC_Date);
	lv_calendar_set_today_date(calendar, &today);    //设置当前日期
	lv_calendar_set_showed_date(calendar, &today);	//设置显示当前日期
	/******tab2******/
	tab2_list = lv_list_create(tab2, NULL);		//创建一个列表
    lv_obj_set_size(tab2_list, 300, 380);
	lv_obj_align(tab2_list, NULL, LV_ALIGN_CENTER, 0, 0);
	tab2_list_btn = lv_list_add_btn(tab2_list,LV_SYMBOL_REFRESH,tab2_dev_list[0]); 
	lv_obj_set_event_cb(tab2_list_btn, tab2_list_handler);
	
	/******tab3******/
    tab3_list = lv_list_create(tab3, NULL);		//创建一个列表
    lv_obj_set_size(tab3_list, 300, 380);
    lv_obj_align(tab3_list, NULL, LV_ALIGN_CENTER, 0, 0);
//	tab3_list_btn = lv_list_add_btn(tab3_list,LV_SYMBOL_EYE_OPEN,tab3_set_list[0]); //列表标签要显示的文字与图像
//	lv_obj_set_event_cb(tab3_list_btn, tab3_list_handler);							//列表按钮与回调函数
	tab3_list_btn = lv_list_add_btn(tab3_list,LV_SYMBOL_WIFI,	 tab3_set_list[0]); 
	lv_obj_set_event_cb(tab3_list_btn, tab3_list_handler);
	tab3_list_btn = lv_list_add_btn(tab3_list,LV_SYMBOL_HOME,	 tab3_set_list[1]); 
	lv_obj_set_event_cb(tab3_list_btn, tab3_list_handler);
	tab3_list_btn = lv_list_add_btn(tab3_list,LV_SYMBOL_SETTINGS,tab3_set_list[2]); 
	lv_obj_set_event_cb(tab3_list_btn, tab3_list_handler);
	tab3_list_btn = lv_list_add_btn(tab3_list,LV_SYMBOL_SETTINGS,tab3_set_list[3]); 
	lv_obj_set_event_cb(tab3_list_btn, tab3_list_handler);
}

void tab2_name_set(char *name)
{
	lv_tabview_set_tab_name(tabview,1,name);
}

//设备刷新
//mode:1 添加 0 清空
void tab2_dev_add(char num,int mode)
{
	if(mode) {
		if(num>0) {
			tab2_list_btn = lv_list_add_btn(tab2_list,LV_SYMBOL_SETTINGS,tab2_dev_list[num]); 
			lv_obj_set_event_cb(tab2_list_btn, tab2_list_handler);
		}
	}
	else {
		lv_list_clean(tab2_list);
	}
}

uint16_t get_tab_act(void)
{
	return lv_tabview_get_tab_act(tabview);
}

//更新日期
void update_time(void)
{
	today.year  = BCD2DEC(RTC_DateStructure.RTC_Year)+2000;
	today.month = BCD2DEC(RTC_DateStructure.RTC_Month);
	today.day   = BCD2DEC(RTC_DateStructure.RTC_Date);
	lv_calendar_set_today_date(calendar, &today);    //设置当前日期
	lv_calendar_set_showed_date(calendar, &today);
}

//更新温湿度等信息
void updata_thfg(float T,float H,uint32_t F,uint32_t G)
{
	if(tab_list_page_now == TAB2USRPAGE+3)
	{
		char msg[50] = {0};
		lv_obj_t *label;
		lv_obj_clean(btn1);
		sprintf(msg,"temp %.2f  Humi %.2f\nFlame %d       Gas %d",T,H,F,G);
		label = lv_label_create(btn1,NULL);
		lv_label_set_text(label,msg);
		lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0 ,0);
	}
}



