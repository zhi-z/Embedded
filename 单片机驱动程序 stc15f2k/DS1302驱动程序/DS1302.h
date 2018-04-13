/*******************************************************************************
* 文件名 : DS1302.h
* 实现功能:
* 版本     作者            日期            说明
* V1.0     zhi-z            2015/08/30       初始版本

* 描述   : MCU: STC15F2K60S2     晶振:22.1184    MHz
           
*******************************************************************************/  

#ifndef __DS1302_H__
#define __DS1302_H__

//sbit SCLK =  P1^1;//串行时钟输入端口
//sbit DAT  =  P1^2;//数据IO端口
//sbit RST  =  P1^3;//复位输入端

sbit SCLK =  P4^4;//串行时钟输入端口
sbit DAT  =  P4^2;//数据IO端口
sbit RST  =  P4^1;//复位输入端



#define		DS1302_SEC_ADD			0x80//秒
#define		DS1302_MIN_ADD			0x82//分
#define		DS1302_HR_ADD			0x84//时
#define		DS1302_DAY_ADD			0x86//日
#define		DS1302_MONTH_ADD		0x88//月
#define		DS1302_WEEK_ADD			0x8A//星期
#define		DS1302_YEAR_ADD			0x8C//年

#define		DS1302_CONTROL_ADD		0x8E//控制
#define		DS1302_CHARGER_ADD		0x90//涓流充电					 
#define		DS1302_CLKBURST_ADD		0xBE//时钟多字节



void DS1302_Init();													//初始化DS1302

void DS1302_Data_Input(unsigned char Address,unsigned char Data);	//数据输入
unsigned char DS1302_Data_Output(unsigned char Address);			//数据输出

void DS1302_Write_Time(void);										//写入初始化时间
void DS1302_Read_Time(void);										//读取时间

#endif