/*******************************************************************************
* 文件名 : lcd12864.h

* 版本     作者            日期            说明
* V1.1     zhi-z        2014/08/03       初始版本

* 描述   : MCU:stc15f2k60s2      晶振:22.1184   MHz
           
******************************************************************************/
#ifndef __lcd12864_H_
#define __lcd12864_H_

#include<STC15F2K60S2.h>


#define byte unsigned char
#define uint unsigned int
#define port P0

sbit lcd_rs = P2^2;
sbit lcd_rw = P2^1;
sbit lcd_e = P2^0; 
sbit lcd_12864_psb = P2^3;

void lcd_delay1ms(unsigned int x);		   //延时函数
void write_com(unsigned char com);		   //写命令函数
void write_dat(unsigned char dat);		   //写数据函数
void lcd_init();						   //初始化
void display_12864(byte x,byte y, byte * p); //显示位置
void place(byte x,byte y);					 //显示

#endif