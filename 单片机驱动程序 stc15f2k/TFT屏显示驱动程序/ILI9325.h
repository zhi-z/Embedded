
/*******************************************************************************
* 文件名 : ILI9325.h

* 版本     作者            日期            说明
* V1.0                  2015/07       初始版本

* 描述   : MCU: STC15F2K60S2     晶振: 22.1184   MHz
           
*******************************************************************************/


#ifndef __ILI9325_H__
#define __ILI9325_H__

#include <STC15F2K60S2.H>

//#define  U8 unsigned char 
//#define  U16 unsigned int

//============================================================
//根据芯片资料定义
#define WINDOW_XADDR_START	0x0050 // Horizontal Start Address Set
#define WINDOW_XADDR_END	0x0051 // Horizontal End Address Set
#define WINDOW_YADDR_START	0x0052 // Vertical Start Address Set
#define WINDOW_YADDR_END	0x0053 // Vertical End Address Set
#define GRAM_XADDR		0x0020 // GRAM Horizontal Address Set
#define GRAM_YADDR		0x0021 // GRAM Vertical Address Set
#define GRAMWR 			0x0022 // memory write
#define GUI_LCM_YMAX 319
#define GUI_LCM_XMAX 239 

sbit TFT_RS=P2^2;	//数据/命令选择
sbit TFT_WR=P2^1;	//写
sbit TFT_RD=P2^0;	//读
sbit TFT_CS=P2^6;	//片选
sbit TFT_RST=P2^7;	//复位

#define TFT_DATA P0	//数据口

extern void ILI9325_initial( );//TFT初始化
extern void TFT_write_cmd(unsigned char CH,unsigned char CL);//写命令
extern void TFT_write_data(unsigned char DH,unsigned char DL);//写两个字节数据
extern void TFT_write_data_u16(unsigned int dat);//写16位数据,通过调用TFT_write_data实现
extern void TFT_write_cmd_data (unsigned char cmd,unsigned int dat);//写命令数据
extern void TFT_setxy(unsigned char x,unsigned int y);//设定当前显示坐标
extern void TFT_delayms(unsigned int z);  //毫秒延时函数
extern void TFT_setwindow(unsigned char x0,unsigned int y0,unsigned char x1,unsigned int y1);//设置窗口
extern void TFT_test();//彩条、单色自测程序
                          
#endif /*__ILI9325_H__*/














