
/*******************************************************************************
* 文件名 : ILI9325.c

* 版本      作者            日期            说明
* V1.0      zhi-z           2015/07       初始版本

* 描述   : MCU: STC15F2K60S2     晶振: 22.1184   MHz
           
*******************************************************************************/

#include "ILI9325.h"

/*******************************************************************************
* 功能描述 : TFT初始化
* 函数属性 : 外部
* 输入参数 : 无
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void ILI9325_initial( )
{ 
	TFT_CS=1;
	TFT_delayms(5);
	TFT_RST=0;
	TFT_delayms(5);
	TFT_RST=1;
	TFT_delayms(50);//可以适当调整延时，保障稳定显示
	TFT_write_cmd_data(0x0001,0x0100); //源扫描设置SS=0, 输出的移动方向是从S1到S
	                                   //720
	                                   
	//（源扫描设置SS=1, 输出的移动方向是从S720到S1）
	//SM=0，逐行扫描（SM=1，隔行扫描）

	TFT_write_cmd_data(0x0002,0x0700); //B/C=1、EOR=1：行倒置（B/C=0：帧/场倒置）
	TFT_write_cmd_data(0x0003,0x1030); //AM=0，地址在水平方向自动加1
	//I/D[1:0] = 11：水平垂直方向均增加
	//BGR=1:采用BGR格式
	//TRI=0、DFM=*：8位机、两次数据传送
	TFT_write_cmd_data(0x0004,0x0000); //比例缩放设置：不缩放
	
	//显示控制
	TFT_write_cmd_data(0x0008,0x0207); //设置后边沿和前沿 
	TFT_write_cmd_data(0x0009,0x0000);//设置非显示区时间间隔ISC[3:0]
	TFT_write_cmd_data(0x000A,0x0000); //帧标记功能

	TFT_write_cmd_data(0x000C,0x0000);//RGB显示接口控制1
	TFT_write_cmd_data(0x000D,0x0000);//帧标记位置
	TFT_write_cmd_data(0x000F,0x0000);//RGB显示接口控制2
	
	//电源配置
	TFT_write_cmd_data(0x0010,0x0000);   
	TFT_write_cmd_data(0x0011,0x0007);  
	TFT_write_cmd_data(0x0012,0x0000);  
	TFT_write_cmd_data(0x0013,0x0000); 
	//vgh 
	TFT_write_cmd_data(0x0010,0x1290);   
	TFT_write_cmd_data(0x0011,0x0227);
	//delayms(100);
	//vregiout 
	TFT_write_cmd_data(0x0012,0x001d); //0x001b
	//delayms(100); 
	//vom amplitude
	TFT_write_cmd_data(0x0013,0x1500);
	//delayms(100); 
	//vom H
	TFT_write_cmd_data(0x0029,0x0018); 
	TFT_write_cmd_data(0x002B,0x000D); 
	
	//伽马校正
	TFT_write_cmd_data(0x0030,0x0004);
	TFT_write_cmd_data(0x0031,0x0307);
	TFT_write_cmd_data(0x0032,0x0002);//0006
	TFT_write_cmd_data(0x0035,0x0206);
	TFT_write_cmd_data(0x0036,0x0408);
	TFT_write_cmd_data(0x0037,0x0507); 
	TFT_write_cmd_data(0x0038,0x0204);//0200
	TFT_write_cmd_data(0x0039,0x0707); 
	TFT_write_cmd_data(0x003C,0x0405);//0504
	TFT_write_cmd_data(0x003D,0x0F02); 
	
	//窗口设置
	TFT_write_cmd_data(0x0050,0x0000);//水平起始位置
	TFT_write_cmd_data(0x0051,0x00EF);//水平终止位置
	TFT_write_cmd_data(0x0052,0x0000);//垂直起始位置
	TFT_write_cmd_data(0x0053,0x013F);//垂直终止位置

	TFT_write_cmd_data(0x0060,0xA700);//门扫描设置，GS=1：从G320扫描到G1，320线
	TFT_write_cmd_data(0x0061,0x0001); 
	TFT_write_cmd_data(0x006A,0x0000);
	//
	TFT_write_cmd_data(0x0080,0x0000); 
	TFT_write_cmd_data(0x0081,0x0000); 
	TFT_write_cmd_data(0x0082,0x0000); 
	TFT_write_cmd_data(0x0083,0x0000); 
	TFT_write_cmd_data(0x0084,0x0000); 
	TFT_write_cmd_data(0x0085,0x0000); 
	//
	TFT_write_cmd_data(0x0090,0x0010); 
	TFT_write_cmd_data(0x0092,0x0600); 
	TFT_write_cmd_data(0x0093,0x0003); 
	TFT_write_cmd_data(0x0095,0x0110); 
	TFT_write_cmd_data(0x0097,0x0000); 
	TFT_write_cmd_data(0x0098,0x0000);
	TFT_write_cmd_data(0x0007,0x0133);	
}


/*******************************************************************************
* 功能描述 : TFT写命令
* 函数属性 : 外部
* 输入参数 : CH 命令的高字节, CL 命令的低字节
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void TFT_write_cmd(unsigned char CH,unsigned char CL)
{
	TFT_CS=0;
	TFT_RS=0;
	TFT_DATA=CH;
	TFT_WR=0;
	TFT_WR=1;
	TFT_DATA=CL;
	TFT_WR=0;
	TFT_WR=1;
	TFT_CS=1;
}


/*******************************************************************************
* 功能描述 : TFT写两个字节的数据
* 函数属性 : 外部
* 输入参数 : DH,DL分别为数据字的高8位和低8位
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void TFT_write_data(unsigned char DH,unsigned char DL)
{
	TFT_CS=0;
	TFT_RS=1;
	TFT_DATA=DH;
	TFT_WR=0;
	TFT_WR=1;
	TFT_DATA=DL;	
	TFT_WR=0;
	TFT_WR=1;
	TFT_CS=1;
}


/*******************************************************************************
* 功能描述 : TFT写两个字节的数据(16位数据)
* 函数属性 : 外部
* 输入参数 : dat,要写入的16位的数据
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void TFT_write_data_u16(unsigned int dat)
{
	TFT_write_data(dat>>8,dat);
}


/*******************************************************************************
* 功能描述 : TFT写命令和写数据
* 函数属性 : 外部
* 输入参数 : cmd,一个字节的命令;  dat,一个字节的数据
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void TFT_write_cmd_data (unsigned char cmd,unsigned int dat)
{
	TFT_write_cmd(0x00,cmd);
	TFT_write_data(dat>>8,dat);
}

/*******************************************************************************
* 功能描述 : TFT设置窗口,设定用来显示的窗口的位置和大小
* 函数属性 : 外部
* 输入参数 : x0 水平起始位置; x1 水平终止位置; y0 垂直起始位置; y1 垂直终止位置
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void TFT_setwindow(unsigned char x0,unsigned int y0,unsigned char x1,unsigned int y1)
{
	TFT_write_cmd_data(WINDOW_XADDR_START,x0);//水平起始位置
	TFT_write_cmd_data(WINDOW_XADDR_END,x1);//水平终止位置
	TFT_write_cmd_data(WINDOW_YADDR_START,y0);//垂直起始位置
	TFT_write_cmd_data(WINDOW_YADDR_END,y1);//垂直终止位置
	TFT_write_cmd_data(GRAM_XADDR,x0);//设置X坐标位置
	TFT_write_cmd_data(GRAM_YADDR,y0);//设置Y坐标位置
	TFT_write_cmd(0x00,0x22);//LCD_WriteCMD(GRAMWR);
}


/*******************************************************************************
* 功能描述 : TFT设定要显示的点的坐标,设定要显示的一个像素点的位置
* 函数属性 : 外部
* 输入参数 : x 像素点的横坐标; y 像素点的纵坐标 
* 返回参数 : 无
* 函数详解 : 本函数实际上是LCD_setwindow函数的一个特例，显示窗口为最小，仅是一个
             像素点。
*******************************************************************************/

void TFT_setxy(unsigned char x,unsigned int y)
{
	TFT_setwindow(x,y,x,y);	
}

/*******************************************************************************
* 功能描述 : 彩条、单色自测程序,需要调用时请去掉注释
* 函数属性 : 外部
* 输入参数 : 无
* 返回参数 : 无
* 函数详解 : 在TFT屏上轮流显示彩条和各种颜色的单色屏幕。
*******************************************************************************/

///*颜色数值:红色，绿色，蓝色，黄色，黑色，白色，天蓝色，紫色*/
//U16 code color[]={0xf800,0x07e0,0x001f,0xffe0,0x0000,0xffff,0x07ff,0xf81f};

//void TFT_test()
//{
//	U16 temp,num;
//	U8 n,c;

//	//显示彩条	
//	TFT_setwindow(0,0,239,319);
//	for(n=0;n<8;n++)
//	{
//		temp=color[n];										
//		for(num=40*240;num>0;num--)
//		TFT_write_data_u16(temp);
//	}
//	TFT_delayms(1000);

//	//显示单色彩色
//	for(c=0;c<8;c++)
//	{
//		temp = color[c];
//		TFT_setwindow(0,0,239,319);
//		for(n=0;n<240;n++)
//		{
//			for(num=0;num<320;num++)
//			TFT_write_data_u16(temp);
//		}
//		TFT_delayms(1000);
//	}
//}

/*******************************************************************************
* 功能描述 : 毫秒延时函数
* 函数属性 : 外部
* 输入参数 : z,延时参数
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void TFT_delayms(unsigned int z)
{
	unsigned int i , j ;
	for (i = z ;i > 0 ; i--)
		for(j = 1700 ; j > 0 ; j-- );
}