


#include"lcd12864.h"


//********************************
void lcd_delay1ms(uint z)//延时函数
{
	unsigned int i , j ;
	for (i = z ; i > 0 ; i--)
		for (j = 1700 ; j > 0 ; j--) ;
}

/****************写命令函数************/
void write_com(byte com)
{
	lcd_rs = 0;
	lcd_rw = 0;
	lcd_e = 0;
	port = com;
	lcd_delay1ms(5);
  lcd_e = 1;
	lcd_delay1ms(5);
	lcd_e = 0;
}

/**************写数据函数***************/
void write_dat(byte dat)
{
	lcd_rs = 1;
	lcd_rw = 0;
	lcd_e = 0;
  port = dat;
	lcd_delay1ms(5);
	lcd_e = 1;
	lcd_delay1ms(5);
	lcd_e = 0;

}
/**************lcd的初始化函数************/
void lcd_init()
{
   lcd_12864_psb = 1;
	 write_com(0x30);//基本指令操作
	 lcd_delay1ms(5);
	 write_com(0x0c);//显示开，关光标
	 lcd_delay1ms(5);
	 write_com(0x01);//清除lcd的显示内容
	 lcd_delay1ms(5);
}
/**************显示位置*********************/

void place(byte x,byte y)
{
	byte psb;
	if(x == 1)
	{
		x = 0x80;
	}

	if(x == 2)
	{
		x = 0x90;
	}

	if(x == 3)
	{
		x = 0x88;
	}

	if(x == 4)
	{
		x = 0x98;
	}
	
	psb = x + y;		   //显示第几个
    write_com(psb);

}

/**************显示*********************/
void display_12864(byte x,byte y, byte * p)
{
	place(x,y);
	while( *p != '\0')
	{
		write_dat(*p);
		p++;
	}
}
