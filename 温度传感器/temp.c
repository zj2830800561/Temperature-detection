#include<temp.h>

//延时函数 （单位：ms）
void delay1ms(uint time)
{
		uchar x;
		for(; time > 0; time--)
		{
					for(x = 110; x > 0; x--);
		}
}

//	初始化：返回值0表示温度传感器不存在，为1表示存在
uchar DS18B20_Init()
{
		uchar time;
		//(1)、数据线拉到低电平“0”
		DSPORT = 0;
		//(2)、延时480微秒（该时间的时间范围可以从480到960微妙）。
		time = 70;
		while(time--);//642us
		//(3)、数据线拉到高电平“1”
		DSPORT = 1;
		//(4)、延时等待80微秒
		time = 0;
		//根据传感器返回值0 || 1 判断是否成功 + 超时判断
		while(DSPORT)
		{
					delay1ms(1);
					time++;
					if(time>5)
								return 0;			//返回初始化失败标志
		}
		return 1;									//返回初始化成功标志
}
//	写数据：	数据从低位到高位的顺序发送
void DS18B20_WriteByte(uchar dat)
{
		uchar i, time = 0;
		for(i = 0; i < 8; i++)
		{
					//(1)、数据线先置低电平“0”
					DSPORT = 0;
					//(2)、延时15微妙。
					time++;
					//(3)、按从低位到高位的顺序发送数据(一次只发送一位)。
					DSPORT = dat & 0x01;			//按位与运算
					//(4)、延时60微妙。
					time = 6;
					while(time--);						//约68us
					//(5)、将数据线拉到高电平。
					DSPORT = 1;
					//数据移位，即下次发送下一位数据
					dat >>= 1;
			
		}
}
//	读数据： 数据从低位到高位开始读取
uchar DS18B20_ReadByte()
{

			uchar i, time = 0;
			uchar data_bit, data_byte;
			//重复读取1位数据，直到读完一个字节
			for(i = 0; i < 8; i++)
			{
						//(1)、将数据线拉低“0”
						DSPORT = 0;
						//(2).延时1微秒。
						time++;
						//(3)、将数据线拉高“1”,释放总线准备读数据。
						DSPORT = 1;
						//(4).延时10微秒。
						time++;
						time++;
						//(5).读数据线的状态得到1个状态位，并进行数据处理。
						data_bit = DSPORT;
						//				：byte右移一位，最高位即为bit左移7位得到的有效数据
						data_byte = (data_byte >> 1) | (data_bit << 7);
						//(6).延时45微妙。
						time = 4;
						while(time--);
			}
			return data_byte;
}
//	温度变换： 使BS18B20改变温度
void DS18B20_Change_Temp()
{
			DS18B20_Init();										//温度传感器初始化
			delay1ms(1);											//延时1毫秒
			DS18B20_WriteByte(0xcc);					//忽略64位rom，直接向温度传感器发送命令
			DS18B20_WriteByte(0x44);					//启动温度变换，转换时长750ms
}
//	发送读取指令： 准备读取温度数据
void DS18B20_Read_Temp_Command()
{
			DS18B20_Init();										//温度传感器初始化
			delay1ms(1);											//延时1毫秒
			DS18B20_WriteByte(0xcc);					//忽略64位rom，直接向温度传感器发送命令
			DS18B20_WriteByte(0xbe);					//发送读暂存器指令，准备读取温度数据
}
//	读取数据：
int DS18B20_Read_Temp()
{
			int temp = 0;											//	将返回的温度值
			uchar temp_low, temp_high;				//	两个字节的值，将组合成温度值
			DS18B20_Change_Temp();						//	使BS18B20改变温度
			DS18B20_Read_Temp_Command();			//	准备读取温度数据
			temp_low = DS18B20_ReadByte();		//	低字节
			temp_high = DS18B20_ReadByte();		//	高字节
			temp = temp_high;									//	将高位数据temp_high先写入temp
			temp = (temp<<8) | temp_low;			//	或运算加入temp_low至温度值中
			return temp;											//	返回温度值
}