/*******************************************************************************
* 文件名 : DHT11.C

* 版本     作者            日期            说明
* V1.1     zhi-z       2015/08/03       初始版本

* 描述   : MCU:stc15f2k60s2      晶振:22.1184   MHz
           
******************************************************************************/

# include <DHT11.h>
#include "uart.h"

unsigned char RH_H; //湿度的高位（整数位）
unsigned char RH_L; //湿度的低位（小数位）

unsigned char T_H;  //温度的高位（整数位）
unsigned char T_L;  //温度的低位（小数位）

unsigned char chack_DATA; //用于检验数据是否正确

unsigned char temperature[] = "00℃";
unsigned char humidity[] = "00%";

/*******************************************************************************************
*	  
*
*				   延时函数	  20ms 50us  30us 
***************************************************************************************************/
void Delay20ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 1;
	j = 216;
	k = 35;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}




void Delay50us()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	i = 1;
	j = 134;
	do
	{
		while (--j);
	} while (--i);
}



void Delay30us()		//@11.0592MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 80;
	while (--i);
}





/*******************************************************************************
* 函数描述 : DHT11读字节函数
* 隶属模块 : 内部
* 输入参数 : 无
* 返回参数 : unsigned char buf , 读回字节数据
*******************************************************************************/
unsigned char DHT11_Read_Byte()
{
	unsigned char buf = 0; //用于缓存数据，默认为0
	unsigned char timer_out = 0; //防止等待时间过长
	unsigned char i = 0;
	for(i=0;i<8;i++)
	{
		DHT11_DATA = 1;
		timer_out = 0;
		while((!DHT11_DATA)&&(timer_out++ < 500));
		//低电平等待，等到高电平出现，同时timer_out防止等待超时
		 
		Delay30us();
		
		buf = buf << 1; //读取到数据‘0’
		if(DHT11_DATA)
		{
			 buf = buf|0x01; //读取数据‘1’
		}
		
		timer_out = 0;
		while((DHT11_DATA)&&(timer_out++ < 500));
		//高电平等待，等到低电平出现，同时timer_out防止等待超时
	}
	
	return buf;
}


/*******************************************************************************
* 函数描述 : DHT11 读温湿度函数
* 隶属模块 : 外部
* 输入参数 : 无
* 返回参数 : unsigned char 1 或 0 , 1 代表读取成功 ,0 代表读取失败
*******************************************************************************/
                                                                                             
unsigned char DHT11_DHT11_Read()
{
	unsigned char timer_out = 0; //防止等待时间过长

	DHT11_DATA = 0;    //主机发开始信号
	Delay20ms(); //开始信号持续的时间
	DHT11_DATA = 1;    //主机拉高等待DHT11响应
	Delay50us(); //拉高等待DHT11响应持续的时间
	
	if(!DHT11_DATA)    //出现低电平，DHT11响应
	{
		timer_out = 0;
		while((!DHT11_DATA)&&(timer_out++ < 500)); //时序图中等待DHT响应80us，
										//      后下面等待DHT拉高后80us等待拉低开始读数据
										//低平等待，等到高电平出现，同时timer_out防止等待超时
		
		timer_out = 0;
		while((DHT11_DATA)&&(timer_out++ < 500));
		//高电平等待，等到低电平出现，同时timer_out防止等待超时
		
		//以下程序是获取温湿度数据 8bit读取
		RH_H = DHT11_Read_Byte(); 
		RH_L = DHT11_Read_Byte();	
		T_H  = DHT11_Read_Byte();
		T_L  = DHT11_Read_Byte();

		chack_DATA = DHT11_Read_Byte(); //检验数据
		
		if(chack_DATA == RH_H + RH_L + T_H + T_L)
		{
			return 1;
		}
		else //检验发现出错
		{
			return 0; //检验失败
		}
	}
	
	else //DHT11没有响应
	{
		return 0; 
	}
}


/*******************************************************************************
* 函数描述 : 显示温湿度函数,需要调用12864显示函数
* 隶属模块 : 外部
* 输入参数 : 无
* 返回参数 : 无
*******************************************************************************/
void display()
{
	if(DHT11_DHT11_Read() == 1)	 //等待是否读取到数据如果返回1则校验成功，可以显示数据。
	{
		if(RH_H > 99)
			RH_H = 0;
		if(T_H > 99)
			T_H = 0;

	/*****************显示温度***********************/
	temperature[0] = T_H/10 + '0';
	temperature[1] = T_H%10 + '0';
	display_12864(2,4, temperature);
	/*****************显示湿度***********************/
	humidity[0] = RH_H/10 + '0';
	humidity[1] = RH_H%10 + '0'; 
	display_12864(3,4, humidity);
	display_12864(4,0, "        ");

	Delay20ms();
	Delay20ms();
	Delay20ms();
	Delay20ms();
	Delay20ms();

	}
	else
		display_12864(4,0, "检测失败");


}




