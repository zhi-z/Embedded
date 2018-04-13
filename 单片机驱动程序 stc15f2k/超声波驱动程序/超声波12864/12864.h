
/*******************************************************************************
* 文件名 : 12864.h

* 版本     作者            日期            说明
* V1.0     zhi-z        2014/08/04       初始版本

* 描述   : MCU: STC15F2K60S2     晶振: 22.1184   MHz
           
*******************************************************************************/



#ifndef __12864_H__
#define __12864_H__

#include "STC15F2K60S2.h"

//第一行显示的内容	 地址0x80—0x87
//第二行显示的内容	 地址0x90—0x97
//第三行显示的内容	 地址0x88—0x8f
//第四行显示的内容	 地址0x98—0x9f

extern void LCD_12864_INIT() ;       //液晶初始化函数
extern void LCD_12864_write_POS(unsigned char x , unsigned char y );//写位置函数
extern void close_LCD_12864_back_light(); //关闭液晶背光
extern void open_LCD_12864_back_light();   //打开液晶背光
extern void show_Typeface(unsigned char x,unsigned char y,unsigned char *p); //显示字体 字符
							// x ,y 为字体 字符的行,列坐标 ,p为字符串指针

void LCD_12864_write_dat(unsigned char dat);   //写数据函数
void LCD_12864_write_cmd(unsigned char cmd);   //写指令函数
void LCD_12864_delay(unsigned int z);  	//延时函数	

sbit LCD_12864_RS = P2^2 ;  		//  指令/数据选择端
sbit LCD_12864_RW = P2^1 ;  		//  读/写选择端
sbit LCD_12864_EN = P2^0 ;		//	使能端
//sbit LCD_12864_PSB = P2^3 ;		//  并/串口选择端	
sbit LCD_12864_back_light  = P2^7; 	//  背光控制	

#endif /*__12864_H__*/


 
