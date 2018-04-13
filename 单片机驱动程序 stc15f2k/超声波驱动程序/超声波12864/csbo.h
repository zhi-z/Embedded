/***************************************************
* 时间：2015.8.5
* 名称：csbo.h
* 作者：zhi-z
* 目的：
* 作用：超声波模块测距离
***************************************************/

#ifndef __csbo_H_
#define __csbo_H_

#include "intrins.h"
#include "stc15f2k60s2.h"


sbit Trig = P3^5;	//发射端
sbit Echo = P3^4;	//接收端

extern int CSB();   //计算测量的距离，返回测量值。




#endif