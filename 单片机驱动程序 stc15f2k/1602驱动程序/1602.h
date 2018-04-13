/*********************************************************************************
* 文件名     :  1602.h
* 描述       :  1602接口描述头文件
 
* 版本     作者            日期            说明
* V1.0      zhi-z       2015/08/5     初始版本
*********************************************************************************/

#ifndef __1602_H__
#define __1602_H__

#include <stc15f2k60s2.H>

extern void delay (unsigned int z);
extern void init();


void write_com (char com);						 //写命令
void write_data (char date)	;					//写数据
void show(unsigned char x,unsigned char *p);	   //显示函数，x= 0x80,0xc0第一第二行


sbit rs = P2^2 ;			//写命令、数据选择端
sbit rw = P2^1 ;		//读写控制端
sbit cs = P2^0 ;		//定义使能端

#endif /*__1602_H__*/


 


