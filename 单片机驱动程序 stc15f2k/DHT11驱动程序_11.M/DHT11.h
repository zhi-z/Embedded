/*******************************************************************************
* 文件名 : DHT11.H

* 版本     作者            日期            说明
* V1.1      zhi-z        2014/08/03       初始版本

* 描述   : MCU:stc15f2k60s2      晶振:22.1184    MHz
           
******************************************************************************/
# ifndef _DHT11_H_
# define _DHT11_H_

# include<STC15F2K60S2.h>
# include "intrins.h"
# include"DHT11.h"
# include "lcd12864.h"

sbit DHT11_DATA = P1^7;

unsigned char DHT11_DHT11_Read();	  //DHT11 读温湿度函数
unsigned char DHT11_Read_Byte();	  //DHT11读字节函数
void Delay20ms();					  //延时函数   20ms
void display();						  // 显示温湿度函数

# endif