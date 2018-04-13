/*******************************************************************************
* 文件名 : main.c
* 实现功能:DS1302测试程序
* 版本     作者            日期            说明
* V1.0     zhi-z           2015/08/30       初始版本

* 描述   : MCU: STC15F2K60S2     晶振:22.1184    MHz
          串口显示，初始时间自己定 
*******************************************************************************/  

#include"STC15F2K60S2.h"
#include"DS1302.h"
#include"intrins.h"
#include "uart.h"

unsigned char CurrentTime[16];//当前时间存储数组
extern unsigned char Time_buf[8];//时间缓冲数组

unsigned char sec_buf = 0;	//用来保存上一次的秒数
unsigned char time=0;		//切换数据用的变量



/*******************************************************************************
* 功能描述 : 主函数
* 函数属性 : 
* 输入参数 : 无
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/
void main()
{
	DS1302_Init();			//DS1302初始化
	DS1302_Write_Time();	//初始化数据
	UartInit();				//串口初始化
	while(1)
	{
		DS1302_Read_Time();
		//年
		CurrentTime[0] = Time_buf[0]>>4;	//得到千位
		CurrentTime[1] = Time_buf[0]&0x0F;	//得到百位
		CurrentTime[2] = Time_buf[7]>>4;	//得到十位
		CurrentTime[3] = Time_buf[7]&0x0F;	//得到个位
		
		//月
		CurrentTime[4] = Time_buf[5]>>4;	//得到十位
		CurrentTime[5] = Time_buf[5]&0x0F;	//得到个位
		
		//日
		CurrentTime[6] = Time_buf[4]>>4;	//得到十位
		CurrentTime[7] = Time_buf[4]&0x0F;	//得到个位
		
		//时
		CurrentTime[8] = Time_buf[3]>>4;	//得到十位
		CurrentTime[9] = Time_buf[3]&0x0F;	//得到个位
		
		//分
		CurrentTime[10] = Time_buf[2]>>4;	//得到十位
		CurrentTime[11] = Time_buf[2]&0x0F;	//得到个位
		
		//秒
		CurrentTime[12] = Time_buf[1]>>4;	//得到十位
		CurrentTime[13] = Time_buf[1]&0x0F;	//得到个位
		
		//星期
		CurrentTime[15] = Time_buf[6];		//得到个位

		if(CurrentTime[13]!=sec_buf)		//当前时间与原来时间不同      
		{    
			sec_buf=CurrentTime[13];//秒
			
/*******************年**********************************************/
			UART_Send_Byte(CurrentTime[0]+'0');
			UART_Send_Byte(CurrentTime[1]+'0');
			UART_Send_Byte(CurrentTime[2]+'0');
			UART_Send_Byte(CurrentTime[3]+'0');
			UART_Send_Str("年");
			UART_Send_Str("/");
/*******************月********************************************/
			UART_Send_Byte(CurrentTime[4]+'0');
			UART_Send_Byte(CurrentTime[5]+'0');
			UART_Send_Str("月");
			UART_Send_Str("/");
/*******************日*********************************************/
			UART_Send_Byte(CurrentTime[6]+'0');
			UART_Send_Byte(CurrentTime[7]+'0');
			UART_Send_Str("日");
			UART_Send_Str("\n");
/*******************星期*********************************************/
			UART_Send_Str("星期");
			UART_Send_Byte(CurrentTime[15]+'0');
			UART_Send_Str("\n");
/*******************时************************************************/
			UART_Send_Byte(CurrentTime[8]+'0');
			UART_Send_Byte(CurrentTime[9]+'0');
			UART_Send_Str(":");
/*******************分************************************************/
			UART_Send_Byte(CurrentTime[10]+'0');
			UART_Send_Byte(CurrentTime[11]+'0');
			UART_Send_Str(":");
/*******************秒************************************************/
			UART_Send_Byte(CurrentTime[12]+'0');
			UART_Send_Byte(CurrentTime[13]+'0');
			UART_Send_Str("\n");

			time = 0;
		}
	}
}
