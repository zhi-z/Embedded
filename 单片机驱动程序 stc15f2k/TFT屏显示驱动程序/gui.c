
/*******************************************************************************
* 文件名 : gui.c

* 版本     作者            日期            说明
* V1.0            2015/07       初始版本

* 描述   : MCU: STC15F2K60S2     晶振: 22.1184   MHz
           
*******************************************************************************/

#include "gui.h"


/*******************************************************************************
* 功能描述 : TFT清屏
* 函数属性 : 外部
* 输入参数 : b_color是背景颜色
* 返回参数 : 无
* 函数详解 : 使用背景颜色清除TFT模块屏幕的全部显示内容
*******************************************************************************/
void GUI_clearscreen(unsigned int b_color)
{
	unsigned int i,j;
	GUI_setwindow(0,0,240,320);
	for (i=0;i<320;i++)
	{
		for(j=0;j<240;j++)
		{
			GUI_write_data_u16(b_color);
		}
			
	}
}
/*******************************************************************************
* 功能描述 : 画点函数
* 函数属性 : 外部
* 输入参数 : (x y) 点的坐标,color 是点的颜色 
* 返回参数 : 无
* 函数详解 : 用指定的颜色在指定的坐标位置上画出一个点
*******************************************************************************/

void GUI_Point(unsigned char x, unsigned int y, unsigned int color)
{  
	GUI_setxy(x,y);
	GUI_write_data_u16(color);
}

/*******************************************************************************
* 功能描述 : 画横线
* 函数属性 : 外部
* 输入参数 : (x，y)是横线起点坐标，length是横线长度，color 是横线颜色
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void GUI_HLine(unsigned char x, unsigned int y, unsigned char length, unsigned int color) 
{  
	GUI_setwindow(x,y,x+length-1,y);
	while(length--)
	{
		GUI_write_data_u16(color);//逐点显示，描出水平线
	}		
}
/*******************************************************************************
* 功能描述 : 画竖线
* 函数属性 : 外部
* 输入参数 : (x，y)是竖线起点坐标，high 竖线高度，color 是竖线颜色
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void GUI_RLine(unsigned char x, unsigned int y, unsigned int high, unsigned int color) 
{  
   GUI_setwindow(x,y,x,y+high-1);
   while(high--)
   {
		GUI_write_data_u16(color);// 逐点显示，描出垂直线
   }		 	
}

/*******************************************************************************
* 功能描述 : 画直线（可以画任意方向直线，包括横线、竖线、斜线）。
* 函数属性 : 外部
* 输入参数 : (x1,y1)起点, (x2,y2)终点, color颜色
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void GUI_line(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int color)  
{  
    unsigned int t,row,col; 
	int xerr=0,yerr=0,delta_x,delta_y,distance ,incx,incy; 
    delta_x = x2-x1;//计算坐标增量  
    delta_y = y2-y1;  
    col=x1;  
    row=y1;
	
    if(delta_x>0)
	{
		incx=1;//设置单步方向 
	} 
    else
	{  
		if(delta_x==0)
		{
			incx=0;//垂直线
		}   
		else
		{
			incx=-1;
			delta_x=-delta_x;
		}  
    } 
	
	
    if(delta_y>0)
	{
		incy=1;
	}  
    else 
	{  
        if(delta_y==0)
		{
			incy=0;//水平线
		}   
        else 
		{
			incy=-1;
			delta_y=-delta_y;
		}  
    }
	
    if(delta_x>delta_y) 
	{
		distance=delta_x;//选取基本增量坐标轴
	}  
    else 
	{
		distance=delta_y;
	}  


    for(t=0;t<=distance+1;t++)  
	{                                     //画线输出  
	    GUI_Point(col, row, color);
        xerr+=delta_x;  
        yerr+=delta_y;  
      	if(xerr>distance)
		{  
            xerr-=distance;  
            col+=incx;  
        }  
        if(yerr>distance)
		{  
            yerr-=distance;  
            row+=incy;  
        }  
    }  
}

/*******************************************************************************
* 功能描述 : 显示16位真彩图片(图标)
* 函数属性 : 外部
* 输入参数 : (x，y)是开始点的坐标，length是图片长度，high是图片高度。
* 返回参数 : 无
* 函数详解 : 指定位置上显示图片。图片事先定义在bmp.h中的pic[]数组中
			 
*******************************************************************************/

void GUI_DisPicture(unsigned char x, unsigned int y, unsigned char length, unsigned int high ,unsigned char *pic)
{
    unsigned int tmp=0,num=0;
	GUI_setwindow(x,y,x+length-1,y+high-1);
	num=length*high*2;
	do
	{  
		//16位真彩图片,每个像素点要用16个二进制位来表示,即用两个字节表示,图片的像素
		//点有length*high个,所以数据有length*high*16位
		GUI_write_data(pic[tmp+1],pic[tmp]);
		//每次写入16位的数据,所以需要写入length*high次就能把图片全部写入.
					
	   tmp+=2;		//因为tmp每次增加2,为了让写数据函数执行length*high次,所以			
	}while(tmp<num);//num的初值应为length*high*2
}

/*******************************************************************************
* 功能描述 : 显示英文或数字字符
* 函数属性 : 外部
* 输入参数 : (x，y) 是显示内容的开始坐标，c：显示的字符，
             color:颜色,b_color:背景颜色。
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void GUI_sprintf_char(unsigned char x, unsigned int y,unsigned char c, unsigned int color,unsigned int b_color)
{  
	unsigned char  s_x ,s_y, temp ;
	c = c - 32;
	for( s_y=0 ; s_y < 16 ; s_y++ )
	{
		if(s_y+y<320)
		{
			temp = font16x8[(unsigned int)c*16+s_y] ;
			for( s_x=0 ; s_x<8 ; s_x++ )
			{
				if(x+s_x<240)
				{
					if((temp&(0x80>>(s_x))) == (0x80>>(s_x))  )
					{
						GUI_Point(x+s_x, y+s_y,color) ;
					}
					else
					{
						GUI_Point(x+s_x, y+s_y,b_color) ;
					}
				}
			}
		}
	}
}


/*******************************************************************************
* 功能描述 : 在指定位置上显示英文字符串
* 函数属性 : 外部
* 输入参数 : (x，y) 字符串的开始坐标(字符串的左上角坐标), *s：英文字符串指针
*             color : 字符串颜色, b_color: 背景颜色
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void GUI_sprintf_string(unsigned char x, unsigned int y, unsigned char code *s, unsigned int color,unsigned int b_color)
{  	
    while (*s) 
    {
        GUI_sprintf_char(x, y,*s, color,b_color);
        s++;
        x=x+8;
    }
}

/*******************************************************************************
* 功能描述 : 显示预定义汉字字符串,汉字字符串数据存放在数组中
* 函数属性 : 外部
* 输入参数 : x,y 开始显示的位置坐标; w,h 已预定义好的汉字的宽和高
             s 指向存放预定义汉字的数组的指针; color 汉字字体颜色 ;
             b_color 汉字的背景颜色
* 返回参数 : 无
* 函数详解 : 汉字需事先定义在数组中,数组数据应采用纵向取模方式且不能选用字节倒序
             的方式.
             取模软件 zimoV2.1的版本有问题,宽和高颠倒了,建议选用 zimoV2.2版 
*******************************************************************************/

void GUI_sprintf_HZ(unsigned char x,unsigned int y,unsigned char w,unsigned int h,unsigned char *s ,unsigned int color,unsigned int b_color)
{ 
	unsigned char s_x=0 ,s_y=0, temp=0 , k ;  
	unsigned int n,words;

	words=sizeof(china_char)/(w*h/8);//words,数组中字体个数
	for( n=0 ; n<words ; n++ )   //n=0代表第一个字n=1代表第二个字……
  	{
		///////// /////// 汉字的在竖直方向可分割为 h/8部分,k=0时显示第一部分,
		///////////////// k=1时显示第二部分......
		for(k = 0 ; k < (h/8); k++)	
		{
			//============================ 横向写入w(汉字的宽)列
			for( s_x=0 ; s_x<w ; s_x++ ) //w列递增
			{
				if(s_x+x<240) //判断从显示处向右是否能写一个汉字的宽
				{
				    temp = s[n*(unsigned int)w*(unsigned int)h/8 + s_x + w*k] ;
					
					//--------------- 纵向写8个点,即一个字节
					for( s_y=0 ; s_y<8 ; s_y++ ) //8行递增
					{
						if(y+s_y<320)	//判断从显示处向右是否能写一个汉字的高
						{
							if((temp&(0x80>>(s_y))) == (0x80>>(s_y))  )
							{
								GUI_Point(x+s_x+n*w, y+s_y+8*k,color) ;
							}
							else
							{
								GUI_Point(x+s_x+n*w, y+s_y+8*k,b_color) ;
							}
						}
					}
					//-------------------
				 }
			}
			//=======================================
		}
		////////////////////////
	}
}



/*******************************************************************************
* 功能描述 : 画实心矩形
* 函数属性 : 外部
* 输入参数 : (sx,sy)左上角顶点坐标, (ex,ey)右下角顶点坐标, color颜色
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void GUI_box(unsigned char sx,unsigned int sy,unsigned char ex,unsigned int ey,unsigned int color)
{ 
	unsigned int temp;   
	GUI_setwindow(sx,sy,ex,ey); 
    sx=ex-sx+1;
	sy=ey-sy+1;

	while(sx--)
	{
	 	temp=sy;
		while(temp--)
	 	{	
			GUI_write_data_u16(color);
		}
	}
}

/*******************************************************************************
* 功能描述 : 画空心矩形
* 函数属性 : 外部
* 输入参数 : (sx,sy)左上角顶点坐标, (ex,ey)右下角顶点坐标, color颜色
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void GUI_rectangle( unsigned char sx,unsigned int sy,unsigned char ex,unsigned int ey,unsigned int color)
{
	GUI_line(sx,sy,ex,sy,color);
	GUI_line(sx,sy,sx,ey,color);
	GUI_line(ex,sy,ex,ey,color);
	GUI_line(sx,ey,ex,ey,color);
}

/*******************************************************************************
* 功能描述 : RGB颜色混合
* 函数属性 : 外部
* 输入参数 : R（红色分量）0-255,G（绿色分量）0-255,B（蓝色分量）0-255
* 返回参数 : 混合后的颜色码
* 函数详解 : 通过电脑拾色板获得 R,G,B分量,将分量交给函数处理,函数会返回一个颜色码

*******************************************************************************/

unsigned int RGB(unsigned char R,unsigned char G,unsigned char B)
{
	return((unsigned int)(R&0XF8)<<8|(unsigned int)(G&0XFC)<<3|(unsigned int)(B&0XF8)>>3);
}


/*******************************************************************************
* 功能描述 : 以二进制形式显示unsigned char型数
* 函数属性 : 外部
* 输入参数 : (x y) 要显示的位置坐标,dat 要显示的数,color 是点的颜色,b_color背景色
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void GUI_sprintf_char_to_bit(unsigned char x, unsigned int y,unsigned char dat, unsigned int color,unsigned int b_color)
{  
	unsigned char i ;
	for(i=0;i<8;i++)
	{
		if((dat&(0x80>>i))==(0x80>>i))
		{
			GUI_sprintf_nu(x,y,1,color,b_color) ;
		}
		else 
		{
			GUI_sprintf_nu(x,y,0,color,b_color) ;
		}
		x+=8 ;
	}
}

/*******************************************************************************
* 功能描述 : 以十六进制形式显示unsigned char型数
* 函数属性 : 外部
* 输入参数 : (x，y)是显示内容的左上角坐标，dat 要显示的unsigned char型数，
*			 color:数字的颜色,b_color:背景颜色
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void GUI_sprintf_char_to_hex(unsigned char x, unsigned int y,unsigned char dat, unsigned int color,unsigned int b_color)
{  
	GUI_sprintf_nu(x,y,dat>>4,color,b_color) ;
	x+=8 ;
	GUI_sprintf_nu(x,y,dat&0x0f,color,b_color) ;
}

/*******************************************************************************
* 功能描述 : 以十进制形式显示unsigned char型数
* 函数属性 : 外部
* 输入参数 : (x，y)是显示内容的左上角坐标，dat 要显示的unsigned char型数，
*			 color:数字的颜色,b_color:背景颜色
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void GUI_sprintf_char_to_dec(unsigned char x,unsigned int y,unsigned char dat,unsigned int color,unsigned int b_color)
{
	GUI_sprintf_char(x,y,dat/100+'0',color,b_color);
	GUI_sprintf_char(x+8,y,dat/10%10+'0',color,b_color);
	GUI_sprintf_char(x+16,y,dat%10+'0',color,b_color);
}

/*******************************************************************************
* 功能描述 : 显示1位16进制数
* 函数属性 : 外部
* 输入参数 : (x，y)是显示内容的左上角坐标，dat 要显示的1位16进制数，
*			 color:数字的颜色,b_color:背景颜色
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/

void GUI_sprintf_nu(unsigned char x, unsigned int y,unsigned char dat, unsigned int color,unsigned int b_color)
{  
    unsigned char  s_x=0 ,s_y=0, temp=0 ;
    if (dat>9)
    {
		dat=dat+7;
	} 
    dat = dat+16;
	
    for(s_y=0;s_y<16;s_y++)
    {
		if(s_y+y<320)
		{
			temp = font16x8[(unsigned int)dat*16+s_y] ;
			for( s_x=0 ; s_x<8 ; s_x++ )
			{
				if(x+s_x<240)
				{
					if((temp&(0x80>>(s_x))) == (0x80>>(s_x))  )
					{
						GUI_Point(x+s_x, y+s_y,color) ;
					}
					else
					{
						GUI_Point(x+s_x, y+s_y,b_color) ;
					}
				}
			}
		}
	}
}

/*******************************************************************************
* 功能描述 : 以十进制形式显示unsigned long int型数
* 函数属性 : 外部
* 输入参数 : (x，y)是显示内容的右上角坐标，dat 要显示的unsigned long int型数，
*			 color:数字的颜色,b_color:背景颜色
* 返回参数 : 无
* 函数详解 : 可以显示的数值范围为：0~4,294,967,295(十进制)，也就是说利用该函数，我
			们最多将一个10位十进制数显示在LCD屏上在使用该函数时，不论最终显示的数有
			几位，它都将在LCD屏幕上占10个字符的空间（显示的多位十进制数不足10位时，
			本函数会在有效显示数字前面补空格）
*******************************************************************************/
//(x，y)是显示内容的右上角坐标
void GUI_sprintf_U32_to_dec(unsigned char x, unsigned int y, unsigned long int dat, unsigned int color,unsigned int b_color)
{
	unsigned long int temp; 
	unsigned char  i;
	for(i=0;i<10;i++)
	{
		if(dat!=0)
		{
			temp=dat/10;
			GUI_sprintf_char(x-7,y,dat-temp*10+'0',color,b_color);
			dat=temp;
		}
		else/*往前写满10位，也就是前面都写空格*/
		{
			GUI_sprintf_char(x-7,y,' ',color,b_color);
		}
		x=x-8;/*设置光标在文字插入后自动左移*/
	}
}


/*******************************************************************************
* 功能描述 : 调用一次所有函数,避免编译出现警告
* 函数属性 : 外部
* 输入参数 : 无
* 返回参数 : 无
* 函数详解 : 
*******************************************************************************/
//void Avoid_warning()
//{
//	GUI_Point(0,0,RED);
//	GUI_HLine(0,0,0,RED);
//	GUI_RLine(0,0,0,RED);
//	GUI_line(0,0,0,0,RED);
//	GUI_DisPicture(0,0,0,0,pic);
//	GUI_sprintf_char(0,0,'0',RED,YELLOW);
//	GUI_sprintf_string(0,0,"0",RED,YELLOW);
//	GUI_box(0,0,0,0,RED);
//	GUI_rectangle(0,0,0,0,RED);
//	GUI_sprintf_HZ(0,0,0,0,china_char,RED,YELLOW);
//	RGB(0,0,0);
//	GUI_sprintf_nu(0,0,0,RED,YELLOW);
//	GUI_sprintf_char_to_bit(0,0,0,RED,YELLOW);
//	GUI_sprintf_char_to_dec(0,0,0,RED,YELLOW);
//	GUI_sprintf_char_to_hex(0,0,0,RED,YELLOW);
//	GUI_sprintf_U32_to_dec(240,0,0,RED,YELLOW);
//	GUI_clearscreen(WHITE);
//}


/*******************************************************************************
* 功能描述 : 主函数
* 函数属性 : 
* 输入参数 : 
* 返回参数 : 
* 函数详解 : 
*******************************************************************************/

void main()
{
	ILI9325_initial( );
//	Avoid_warning();	 //调用所有函数
	GUI_clearscreen(WHITE);   //清屏
	GUI_DisPicture(32,30,187,91,pic);
	GUI_sprintf_HZ(56,190,29,32,china_char,BLUE,WHITE);
	while(1);
}

