#ifndef __lcd12864_H_
#define __lcd12864_H_

#include<STC15F2K60S2.h>


#define byte unsigned char
#define uint unsigned int
#define port P0

sbit lcd_rs = P2^2;
sbit lcd_rw = P2^1;
sbit lcd_e = P2^0; 
sbit lcd_12864_psb = P2^3;
void lcd_delay1ms(unsigned int x);
void write_com(unsigned char com);
void write_dat(unsigned char dat);
void lcd_init();
void display_12864(byte x,byte y, byte * p);
void place(byte x,byte y);

#endif