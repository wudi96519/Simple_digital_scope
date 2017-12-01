#include "DSP28x_Project.h"
#include <string.h>
#include "lcd12864.h"
#include <stdio.h>

#define uint8 unsigned char
#define uint32 unsigned int
#define int8 char
#define int32 int
#define uchar unsigned char
#define uint unsigned int

#define BASIC_SET 0x30
#define EXTEND_SET 0x34
#define DRAW_ON 0x36
#define DRAW_OFF 0x34
#define execute_72us delay_Loop(10)
#define SCLK_quarter delay_Loop(1)
#define SCLK_half delay_Loop(2)

#define CS_1 GpioDataRegs.GPADAT.bit.GPIO5 = 1
#define CS_0 GpioDataRegs.GPADAT.bit.GPIO5 = 0
#define SCLK_1 GpioDataRegs.GPADAT.bit.GPIO4 = 1
#define SCLK_0 GpioDataRegs.GPADAT.bit.GPIO4 = 0
#define SDATA_OUT GpioCtrlRegs.GPADIR.bit.GPIO2 = 1
#define SDATA_IN GpioCtrlRegs.GPADIR.bit.GPIO2 = 0
#define SDATA GpioDataRegs.GPADAT.bit.GPIO2
#define RST_1 GpioDataRegs.GPADAT.bit.GPIO3 = 1
#define RST_0 GpioDataRegs.GPADAT.bit.GPIO3 = 0

unsigned char AC_TABLE[] = {
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,\
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,\
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,\
	0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,\
};
static Uint16 frambuff[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int sin[] = {32, 32, 32, 32, 32, 39, 41, 42, 44, 45, 47, 48, 49, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 60, 61, 62, 62, 63, 63, 63, 63, 63, 64, 63, 63, 63, 63, 63, 62, 62, 61, 60, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 49, 48, 47, 45, 44, 42, 41, 39, 38, 36, 35, 33, 32, 30, 28, 27, 25, 24, 22, 21, 19, 18, 16, 15, 14, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16, 18, 19, 21, 22, 24, 25, 27, 28, 30};
char msgF[]="  Freq:   .   Hz \0";
char msgV[]="  Volt:   .   V  \0";

#define uchar unsigned char
#define uint unsigned int

int INDEX_TO_DRAW=0;

extern double freq;
extern double Voltage;

extern void get_trigger_index(int index_num);
extern int index_buff[2];
extern Uint16 SampleTable[];
int votage_Level = 0;

Uint16 Key = 1;
void put_point(uchar x, uchar y);
int frambufftoXADR(int frambuff);
int frambufftoYADR(int frambuff);
void lcd_ready_to_draw();
void lcd_draw_over();

void Delay(uint MS)
{
	uchar us, usn;
	while (MS != 0) //for 12M
	{
		usn = 2;
		while (usn != 0)
		{
			us = 0xf5;
			while (us != 0)
			{
				us--;
			};
			usn--;
		}
		MS--;
	}
}
void DelayKey(uint Second, uint MS100)
{
	uint i;
	for (i = 0; i < Second * 100 + MS100 * 10; i++)
	{
		if (Key == 0)
		{
			Delay(20);
			while (Key == 0)
			{
				Delay(20);
			}
			break;
		}
		else
			Delay(10);
	}
}

void delay_Loop(Uint32 x)
{
	Uint32 i;
	for (i = 0; i < x; i++)
	{
	}
}

Uint16 read_byte(void)
{
	Uint16 i = 0;
	Uint16 data = 0;
	SDATA_IN;
	data = 0;
	for (i = 0; i <= 7; i++)
	{
		SCLK_0;
		SCLK_quarter;
		SCLK_1;
		SCLK_quarter;
		if (SDATA)
		{
			data = (data | (0x01 << (7 - i)));
		}
		SCLK_quarter;
		SCLK_0;
		SCLK_quarter;
	}
	return data;
}

void write_byte(Uint16 x)
{
	Uint16 i = 0;
	SDATA_OUT;
	SCLK_quarter;
	for (i = 0; i <= 7; i++)
	{
		SCLK_0;
		SCLK_quarter;
		SDATA = (0x1 & ((x) >> (7 - i)));
		SDATA = (0x1 & ((x) >> (7 - i)));
		SCLK_quarter;
		SCLK_1;
		SCLK_half;
		SCLK_0;
		SCLK_quarter;
	}
	SDATA = 0;
}

Uint16 lcd_read_data(void)
{
	Uint16 tmp1, tmp2, data;
	CS_1;
	write_byte(0xFE); 
	tmp1 = read_byte();
	tmp2 = read_byte();
	CS_0;
	execute_72us;
	data = (tmp1 & 0xF0);
	data = (data | ((tmp2 & 0xF0) >> 4));
	return data;
}

void lcd_write_data(Uint16 data)
{
	Uint16 tmp1, tmp2;
	CS_1;
	delay_Loop(40);
	tmp1 = (data & 0xF0);
	tmp2 = ((data & 0x0F) << 4);
	write_byte(0xFA);
	write_byte(tmp1);
	write_byte(tmp2);
	delay_Loop(40);
	CS_0;
	execute_72us;
}

void lcd_write_cmd(Uint16 data)
{
	Uint16 tmp1, tmp2;
	CS_1;
	delay_Loop(40);
	tmp1 = (data & 0xF0);
	tmp2 = ((data & 0x0F) << 4);
	write_byte(0xF8);
	write_byte(tmp1);
	write_byte(tmp2);
	delay_Loop(40);
	CS_0;
	execute_72us;
}

void lcd_gpio_init(void)
{
	EALLOW;
	GpioCtrlRegs.GPAMUX1.all = 0x00000000; // All GPIO
	GpioCtrlRegs.GPAMUX2.all = 0x00000000; // All GPIO
	GpioCtrlRegs.GPAMUX1.all = 0x00000000; // All GPIO
	GpioCtrlRegs.GPADIR.all = 0x0000003C;
	GpioCtrlRegs.GPAPUD.all = 0xFFFFFFC3;
	EDIS;
}


void LcmInit(void)
{
	lcd_write_cmd(0x30);
	lcd_write_cmd(0x03);
	lcd_write_cmd(0x0C);
	lcd_write_cmd(0x01);
	lcd_write_cmd(0x06);
}

void Lcd_ClearTXT(void)
{
	uchar i;
	lcd_write_cmd(0x30);
	lcd_write_cmd(0x80);
	for (i = 0; i < 64; i++)
		lcd_write_data(0x20);
}

void Lcd_ClearBMP(void)
{
	uchar i, j;
	lcd_write_cmd(0x34);
	lcd_write_cmd(0x36);
	for (i = 0; i < 32; i++)
	{
		lcd_write_cmd(0x80 | i);
		lcd_write_cmd(0x80);
		for (j = 0; j < 32; j++)
			lcd_write_data(0);
	}
}

void lcd_PutStr(uchar row, uchar col, char *puts)
{
	const char endc[] = {"\0"};
	lcd_write_cmd(0x30);
	lcd_write_cmd(AC_TABLE[8 * row + col]);
	strcat(puts, endc);
	while (*puts != '\0')
	{
		if (col == 8)
		{
			col = 0;
			row++;
		}
		if (row == 4)
			row = 0;
		lcd_write_cmd(AC_TABLE[8 * row + col]);
		lcd_write_data(*puts);
		puts++;
		lcd_write_data(*puts);
		puts++;
		col++;
	}
}
void lcd_PutBMP(Uint16 *puts)
{
	uint x = 0;
	uchar i, j;
	lcd_write_cmd(0x34); 
	lcd_write_cmd(0x36); 
	for (i = 0; i < 32; i++)
	{
		lcd_write_cmd(0x80 | i); 
		lcd_write_cmd(0x80);
		for (j = 0; j < 16; j++) 
		{

			lcd_write_data(((puts[x] & 0xFF00) >> 8));
			lcd_write_data((puts[x] & 0x00FF));
			x++;
		}
	}
}

void lcd_Clean_Screnn_With_Buffer()
{
	int i = 0;
	lcd_ready_to_draw();
	for (i = 0; i < 512; i++)
	{
		if (frambuff[i] != 0x00)
		{

			lcd_write_cmd(frambufftoYADR(i));
			lcd_write_cmd(frambufftoXADR(i));

			lcd_write_data(0x00);
			lcd_write_data(0x00);
			frambuff[i] = 0x00;
		}
	}
	lcd_draw_over();
	lcd_Clear_frambuff();
}

void lcd_Draw_With_Buffer()
{
	int i = 0;
	lcd_ready_to_draw();
	for (i = 0; i < 512; i++)
	{
		if (frambuff[i] != 0x00)
		{

			lcd_write_cmd(frambufftoYADR(i));
			lcd_write_cmd(frambufftoXADR(i));
			lcd_write_data(frambuff[i]>>8);
			lcd_write_data(frambuff[i]);
			//frambuff[i] = 0x00;
		}
	}
	lcd_draw_over();
	//lcd_Clear_frambuff();
}

int xytobuffRange(int x, int y)
{
	//    y = 63 - y;
	int x_byte = x / 16;

	int down = y / 32;
	int buffrange = (y % 32) * 16 + (8 * down + x_byte);
	return buffrange;
}
int frambufftoXADR(int frambuff)
{
	return 0x80 + frambuff % 16;
}

int frambufftoYADR(int frambuff)
{
	return 0x80 + frambuff / 16;
}

void lcd_PutPointtoBuffer(int x, int y)
{
	int bits = (0x8000) >> (x % 16);
	int buffrange = xytobuffRange(x, y);
	frambuff[buffrange] = frambuff[buffrange] | bits;
}

void lcd_Init()
{
	lcd_gpio_init();
	delay_Loop(632000); 
	RST_1;
	RST_1;
	delay_Loop(632000);
	LcmInit();
	Lcd_ClearTXT();
	Lcd_ClearBMP();
}

void lcd_Clear_frambuff()
{
	int i = 0;
	for (i = 0; i < 512; i++)
	{
		frambuff[i] = 0;
	}
}

void lcd_Update()
{
	lcd_PutBMP(frambuff);
	lcd_Clear_frambuff();
}

void lcd_Sin_test()
{
	lcd_Clean_Screnn_With_Buffer();
	int i = 0, oldY = sin[0], j = 0;
	for (j = 0; j < 128; j++)
	{
		if (oldY - sin[j] >= 4)
		{
			for (i = 0; i < oldY - sin[j]; i++)
			{
				put_point(j, sin[j] + i);
			}
		}
		else if (sin[j] - oldY >= 4)
		{
			for (i = 0; i < sin[j] - oldY; i++)
			{
				put_point(j, oldY + i);
			}
		}
		put_point(j, sin[j]);
		oldY = sin[j];
	}
	lcd_Draw_With_Buffer();
}

void lcd_Toast(char *puts)
{
	lcd_PutStr(0, 0, puts);
	int i = 0;
	for(i=0;i<TOAST_TEXT_TIME;i++)
	DELAY_LOOP();
	Lcd_ClearTXT();
}

void DELAY_LOOP()
{
	int i = 0;
	for (; i < TOAST_TIME; i++)
	{
		delay_Loop(65535);
	}
}
void lcd_Draw_Sample(void)
{
	lcd_Clean_Screnn_With_Buffer();
	get_trigger_index(1);
	INDEX_TO_DRAW=index_buff[0];
	if(INDEX_TO_DRAW>=127)
	    INDEX_TO_DRAW=127;
	int j = 0, i = 0;
	Uint16 oldY = (SampleTable[0] >> (6 + votage_Level));
	for (j = 0; j < 128; j++)
	{
		Uint16 sampletodraw = SampleTable[j+INDEX_TO_DRAW] >> (6 + votage_Level);
		if (oldY >= 4 + sampletodraw)
		{
			for (i = 0; i < oldY - sampletodraw; i++)
			{
				put_point(j, sampletodraw + i);
			}
		}
		if (sampletodraw >= 4 + oldY)
		{
			for (i = 0; i < sampletodraw - oldY; i++)
			{
				put_point(j, oldY + i);
			}
		}
		put_point(j, sampletodraw);
		oldY = sampletodraw;
	}
	lcd_Draw_With_Buffer();
}

void lcd_ready_to_draw()
{
	lcd_write_cmd(0x34);
	lcd_write_cmd(0x34);
}

void lcd_draw_over()
{
	lcd_write_cmd(0x36);
	lcd_write_cmd(0x30);
}

/**There's no need to calculate x,y to buffer index
*   Change it later
*/
void put_point(uchar x, uchar y)
{
	y = (y > 63) ? 63 : y;
	y = 63 - y;
	lcd_PutPointtoBuffer(x, y);
//	uint bt = 0;
//	uchar x_adr, y_adr, h_bit, l_bit;
//	y_adr = 0x80 + y % 32;
//	if (y > 31)
//		x_adr = 0x88 + x / 16;
//	else
//		x_adr = 0x80 + x / 16;
//	bt = 0x8000 >> (x % 16);
//	int buffrange = xytobuffRange(x, y);
//	bt = bt | frambuff[buffrange];
//	h_bit = bt >> 8;
//	l_bit = bt;
//	lcd_write_cmd(y_adr);
//	lcd_write_cmd(x_adr);
//	lcd_write_data(h_bit);
//	lcd_write_data(l_bit);
}

void lcd_Show_Freq_Volt(double freq,double Voltage)
{
    if(freq/1000>1)
    {
        freq/=1000;
        msgF[13]='k';
    }
    int i=0;
    msgF[12]='0'+((int)(freq*100))%10;
    msgF[11]='0'+((int)(freq*10))%10;
    msgF[9]='0'+((int)freq)%10;
    msgF[8]='0'+((int)freq)/10%10;
    msgF[7]='0'+((int)freq)/100%10;
    for(i=7;i<=10;i++)
    {
        if(msgF[i]!='0')
            break;
        else
            msgF[i]=' ';
    }
    msgV[9]='0'+(int)Voltage%10;
    msgV[11]='0'+(int)(Voltage*10)%10;
    msgV[12]='0'+(int)(Voltage*100)%10;
    msgV[13]='0'+(int)(Voltage*1000)%10;
    lcd_PutStr(0,0,msgF);
    lcd_PutStr(1,0,msgV);
}
