#include<reg52.h>
#include<temp.h>

sbit LSA = P2^2;
sbit LSB = P2^3;
sbit LSC = P2^4;
sbit beep = P1^5;
sbit OUT1 = P1^0;
sbit OUT2 = P1^1;
// 读取温度函数声明
int DS18B20_Read_Temp();

//	数字0-9段码
uchar code SMG[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
//	温度
uchar global_temp;
uchar Tempdata[6];

//10us=0.01ms
void delay(uint time)
{
			while(time--);
}

void Display()
{
			uchar i;
			for(i = 1; i <= 8; i++)
			{
						switch(i)
						{
									case 1:
										LSA=0;LSB=0;LSC=0;P0 = Tempdata[5];break;
									case 2:
										LSA=1;LSB=0;LSC=0;P0 = Tempdata[4];break;
									case 3:
										LSA=0;LSB=1;LSC=0;P0 = Tempdata[3];break;		//个位
									case 4:
										LSA=1;LSB=1;LSC=0;P0 = Tempdata[2];break;		//十位
									case 5:
										LSA=0;LSB=0;LSC=1;P0 = Tempdata[1];break;
									case 6:
										LSA=1;LSB=0;LSC=1;P0 = Tempdata[0];break;
									case 7:
										LSA=0;LSB=1;LSC=1;P0 = 0x00;break;
									case 8:
										LSA=1;LSB=1;LSC=1;P0 = 0x00;break;
						}
						delay(100);
						P0 = 0x00;
			}
}
//	数据处理： 将返回的int温度值（12位）添加到Tempdata准备数码管显示
void tempdata_handle(int temp)
{
			float tp;
			//如果温度小于0
			if(temp < 0)
			{
						Tempdata[0] = 0x40;											//	显示负号
						temp -= 1;															//	负数补码求原码：补码 - 1 再取反
						temp = ~temp;
						tp = temp;
						temp = tp * 0.0625 * 100;								//	温度数值放大一百倍
			}
			else
			{
						Tempdata[0] = 0x00;											//	不显示符号
						tp = temp;	
						temp = tp * 0.0625 * 100;								//	温度数值放大一百倍
			}
			/*-------======将数据存放至数组中======--------*/
			Tempdata[1] = SMG[temp / 10000];
			Tempdata[2] = SMG[temp % 10000 / 1000];
			Tempdata[3] = SMG[temp % 1000 / 100] | 0x80;		//添加小数点
			Tempdata[4] = SMG[temp % 100 / 10];
			Tempdata[5] = SMG[temp %100 % 10];
			global_temp = (temp % 10000 / 1000) * 10 + (temp % 1000 / 100);
}

void main()
{
			OUT2 = 0x02;
			//配置定时器:256us
			TMOD = 0x02;
			//预置数
			TH0 = 0x00;
			//计数器
			TL0 = 0x00;
			//配置中断
			EA =1;//总中断
			ET0 = 1;//下降沿触发
			TR0 = 1;//定时器0开始工作
	
			tempdata_handle(DS18B20_Read_Temp());
			Display();
}

//定时器0中断
void time_0 () interrupt 1
{
		if(global_temp >= 26){
					// 警报
					beep = ~beep;
					// 风扇
					OUT1 = 0x01;
		}
		else
		{
					OUT1 = 0x00;
		}
}