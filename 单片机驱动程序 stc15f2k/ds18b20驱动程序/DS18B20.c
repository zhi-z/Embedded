/*******************************************************************************
* 文件名 : ds12864.c
* 实现功能:12864测试程序
* 版本     作者            日期            说明
* V1.0     zhi-z            2015/08/30       初始版本

* 描述   : MCU: STC15F2K60S2     晶振: 22.1184   MHz
           
*******************************************************************************/

#include <STC15F2K60S2.H>
#include"DS18B20.H"
#include"intrins.h"
#include"stdio.h"
unsigned char TempTH=0;
unsigned char TempTL=0;

/*******************************************************************************
* 函数描述 : 延时函数
* 隶属模块 : 内部
* 输入参数 : 无
* 返回参数 : 
*******************************************************************************/
static void Delay(unsigned int i)
{
	unsigned int j,k;
	for(j=i;j>0;j--)
		for(k=1600;k>0;k--);
}
void Delay650us()		//@22.1184MHz
{
	unsigned char i, j;

	_nop_();
	i = 14;
	j = 249;
	do
	{
		while (--j);
	} while (--i);
}
void Delay45us()		//@22.1184MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 1;
	j = 244;
	do
	{
		while (--j);
	} while (--i);
}
void Delay480us()		//@22.1184MHz
{
	unsigned char i, j;

	_nop_();
	i = 11;
	j = 80;
	do
	{
		while (--j);
	} while (--i);
}
void Delay2us()		//@22.1184MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 8;
	while (--i);
}
void Delay15us()		//@22.1184MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 80;
	while (--i);
}
void Delay60us()		//@22.1184MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 2;
	j = 70;
	do
	{
		while (--j);
	} while (--i);
}
void Delay10us()		//@22.1184MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	_nop_();
	i = 52;
	while (--i);
}

/*******************************************************************************
* 函数描述 : 初始化
* 隶属模块 : 内部
* 输入参数 : 无
* 返回参数 :  Flag, 用与判断
*******************************************************************************/

bit InitDs18b20(void)
{

	bit Flag;

	DQ = 1;//动释放总线稍作延时
	Delay(2);
	
	DQ = 0;//主机发送复位脉冲
	Delay650us();//至少拉低480us
	
	DQ = 1;//释放总线
	Delay60us();//延时等待应答脉冲
	Flag=DQ;
	if(!Flag)//如果存在应答脉冲再延时480us
	{
		Delay480us();
	}
	return Flag;
}
/*******************************************************************************
* 函数描述 : 写数据。先写低位再写高位
* 隶属模块 : 内部
* 输入参数 : dat
* 返回参数 : 
*******************************************************************************/

void WriteData(unsigned char dat)
{
	unsigned char variable;
	int i=0;
	for(i=0;i<8;i++)
	{
		variable=dat&0x01;
		if(variable)//写1
		{
			DQ = 0;//主机拉低总线至少2us
			Delay2us();
			DQ = 1;//拉高总线等待DS18B20采样
			Delay60us();
		}
		else//写0
		{
			DQ = 0;//主机拉低总线至少60us
			Delay60us();
			DQ = 1;//释放总线
			Delay2us();
		}
		dat=dat>>1;
	}
}

/*******************************************************************************
* 函数描述 : 读数据。
* 隶属模块 : 内部
* 输入参数 : 无
* 返回参数 : dat 
*******************************************************************************/
unsigned char ReadData(void)
{
	unsigned char dat=0;
	int i=0;
	for(i=0;i<8;i++)
	{	
		dat=dat>>1;
		DQ = 0;
		Delay2us();
		DQ = 1;
		Delay10us();
		if(DQ)
		{
			dat=dat|0x80;
		}
		Delay60us();
	}
	return dat;
}

/*******************************************************************************
* 函数描述 : 读温度。
* 隶属模块 : 内部
* 输入参数 : 无
* 返回参数 :  无 
*******************************************************************************/
void Temperature(void)
{	
	if(!InitDs18b20())
	{
		WriteData(0xCC);//跳过ROM操作
		WriteData(0x44);//启动温度转换
		Delay(500);			//延时等待温度转换完成
	}
	if(!InitDs18b20())
	{
		WriteData(0xCC);//跳过ROM操作
		WriteData(0xBE);//读温度
	}
	TempTL=ReadData();//读温度低8位
	TempTH=ReadData();//读温度高8位
	
}

/*******************************************************************************
* 函数描述 : 显示函数。
* 隶属模块 : 内部
* 输入参数 : 无
* 返回参数 : 无 
*******************************************************************************/
void DisplayTemp(void)
{
	int Temp;
	float Temp1;
	Temperature();
	Temp=(((unsigned int)TempTH)<<8)|TempTL;
	if(TempTH&0Xf8)
	{
		Temp=~Temp;
		Temp=Temp+1;
	}
	Temp=Temp&0x07ff;
	Temp1=(float)Temp*0.0625;
	printf("%f\n",Temp1);


}
