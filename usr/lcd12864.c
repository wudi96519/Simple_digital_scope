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

#define FRAMBUFFSIZE 512
#define DWAR_LINE_BAND 4

unsigned char AC_TABLE[] = {
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,\
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,\
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,\
	0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,\
};

Uint16 frambuff[FRAMBUFFSIZE] = {0};

void put_point(uchar x, uchar y);
int frambufftoXADR(int frambuff);
int frambufftoYADR(int frambuff);
void lcd_ready_to_draw();
void lcd_draw_over();


void delay_Loop(Uint32 x)
{
	Uint32 i;
	for (i = 0; i < x; i++)
	{
	}
}

void DELAY_LOOP()
{
	int i = 0;
	for (; i < TOAST_TIME; i++)
	{
		delay_Loop(50000);
	}
}

/* *
 * SampleTable长度可以大于128，
 * 从INDEX_TO_DRAW开始画，
 * 画点长度为LENGTH_TO_DRAW，
 * 缩放（缩小）倍数为lessen_times
 * */
void lcd_Draw_Curve(Uint16 * SampleTable,int INDEX_TO_DRAW,int LENGTH_TO_DRAW,int lessen_times)
{
	if(INDEX_TO_DRAW>=127)
	    INDEX_TO_DRAW=127;
	int j = 0, i = 0;
	Uint16 oldY = (SampleTable[0] >> lessen_times);
	for (j = 0; j < LENGTH_TO_DRAW; j++)
	{
		Uint16 sampletodraw = SampleTable[j+INDEX_TO_DRAW] >> lessen_times ;
		if (oldY >= DWAR_LINE_BAND + sampletodraw)
		{
			for (i = 0; i < oldY - sampletodraw; i++)
			{
				put_point(j, sampletodraw + i);
			}
		}
		if (sampletodraw >= DWAR_LINE_BAND + oldY)
		{
			for (i = 0; i < sampletodraw - oldY; i++)
			{
				put_point(j, oldY + i);
			}
		}
		put_point(j, sampletodraw);
		oldY = sampletodraw;
	}
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
		}
	}
	lcd_draw_over();
}

int xytobuffRange(int x, int y)
{
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

void put_point(uchar x, uchar y)
{
	y = (y > 63) ? 63 : y;
	y = 63 - y;
	lcd_PutPointtoBuffer(x, y);
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

void lcd_Toast(char *puts)
{
	lcd_PutStr(0, 0, puts);
	int i = 0;
	for(i=0;i<TOAST_TEXT_TIME;i++)
	DELAY_LOOP();
	Lcd_ClearTXT();
}

