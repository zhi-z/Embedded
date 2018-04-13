/*******************************************************************************
* 文件名 : ILI9325.h

* 版本     作者            日期            说明
* V1.0     zhi-z           2015/07       初始版本

* 描述   : MCU: STC15F2K60S2     晶振: 22.1184   MHz
           
*******************************************************************************/

#ifndef _gui_h_
#define _gui_h

#include "ILI9325.h"
#include "bmp.h"
#include "16x8.h"


/*定义常用颜色的颜色码*/
#define RED 0xf800	//红
#define YELLOW 0xffe0	//黄
#define GREEN 0x07e0	//绿
#define CYAN 0x07ff	//青
#define BLUE 0x001f	//蓝
#define PURPLE 0xf81f	//紫
#define BLACK 0x0000	//黑
#define WHITE 0xffff	//白
#define GRAY 0x7bef	//灰

//------------------------- GUI与TFT的接口函数宏定义 ---------------------------
#define GUI_write_data       TFT_write_data		//两个形参,写入两个字节的数据
#define GUI_write_data_u16   TFT_write_data_u16			//写16位数据
#define GUI_setxy            TFT_setxy   			 //设定一个像素点的坐标
#define GUI_setwindow        TFT_setwindow			 //设置窗口
//------------------------------------------------------------------------------

//extern void Avoid_warning(); //调用一次,避免出现警告
extern unsigned int RGB(unsigned char R,unsigned char G,unsigned char B);//RGB颜色混合
extern void GUI_clearscreen(unsigned int b_color);//清屏
extern void GUI_Point(unsigned char x, unsigned int y, unsigned int color);//画点
extern void GUI_HLine(unsigned char x, unsigned int y, unsigned char length, unsigned int color);//画横线
extern void GUI_RLine(unsigned char x, unsigned int y, unsigned int high, unsigned int color);//画竖线
extern void GUI_line(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int color);//画直线
extern void GUI_box(unsigned char sx,unsigned int sy,unsigned char ex,unsigned int ey,unsigned int color);//画实心矩形
extern void GUI_rectangle( unsigned char sx,unsigned int sy,unsigned char ex,unsigned int ey,unsigned int color);//画空心矩形
extern void GUI_DisPicture(unsigned char x, unsigned int y, unsigned char length, unsigned int high,unsigned char *pic);//显示图片

//显示1位16进制数
extern void GUI_sprintf_nu(unsigned char x, unsigned int y,unsigned char dat, unsigned int color,unsigned int b_color);

//无符号字符型数以二进制形式显示
extern void GUI_sprintf_char_to_bit(unsigned char x, unsigned int y,unsigned char dat,unsigned int color,unsigned int b_color);

//无符号字符型数以十进制形式显示
extern void GUI_sprintf_char_to_dec(unsigned char x,unsigned int y,unsigned char dat,unsigned int color,unsigned int b_color);

//无符号字符型数以十六进制形式显示
extern void GUI_sprintf_char_to_hex(unsigned char x, unsigned int y,unsigned char dat, unsigned int color,unsigned int b_color);

//显示英文或数字字符
extern void GUI_sprintf_char(unsigned char x, unsigned int y,unsigned char c, unsigned int color,unsigned int b_color);

//显示英文字符串
extern void GUI_sprintf_string(unsigned char x, unsigned int y,unsigned char code *s, unsigned int color,unsigned int b_color);

//显示预定义汉字字符串
extern void GUI_sprintf_HZ(unsigned char x,unsigned int y,unsigned char w,unsigned int h,unsigned char *s,unsigned int color,unsigned int b_color);

//以十进制形式显示无符号长整型数,//(x，y)是显示内容的右上角坐标
extern void GUI_sprintf_U32_to_dec(unsigned char x, unsigned int y, unsigned long int dat, unsigned int color,unsigned int b_color);


#endif













