#ifndef _temp_H
#define _temp_H

#include<reg52.h>

#ifndef uchar
#define uchar unsigned char
	#endif

#ifndef uint
#define uint unsigned int
	#endif

sbit DSPORT = P3^7;

int DS18B20_Read_Temp();

#endif