/*******************************************************************************
* 文件名 : lcd12864.h
* 实现功能:12864驱动程序
* 版本     作者            日期            说明
* V1.0     zhi-z            2015/08/30       初始版本

* 描述   : MCU: STC15F2K60S2     晶振:22.1184    MHz
           
*******************************************************************************/  
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


void write_com(unsigned char com);		//写命令
void write_dat(unsigned char dat);		//写数据
void lcd_init();						//lcd初始化
void display(byte x,byte y, byte * p);	//显示函数,x,y为行列
void place(byte x,byte y);				//显示位置



#endif