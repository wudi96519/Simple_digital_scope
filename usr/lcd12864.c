#include "DSP28x_Project.h"
#include <string.h>
#include "lcd12864.h"

#define uint8 unsigned char
#define uint32 unsigned int
#define int8 char
#define int32 int
#define uchar unsigned char
#define uint unsigned int

#define BASIC_SET 0x30               //基本指令集  00110000
#define EXTEND_SET 0x34              //扩展指令集  00110100
#define DRAW_ON 0x36                 //绘图显示开  00110110
#define DRAW_OFF 0x34                //绘图显示关  00110100
#define execute_72us delay_Loop(400) //执行时间
#define SCLK_quarter delay_Loop(40)  //四分之一SCLK时钟 SCLK是LCD串行通信时钟
#define SCLK_half delay_Loop(80)     //二分之一SCLK时钟 SCLK是LCD串行通信时钟

#define CS_1 GpioDataRegs.GPADAT.bit.GPIO5 = 1      //GPIO5作为CS
#define CS_0 GpioDataRegs.GPADAT.bit.GPIO5 = 0      //GPIO5作为CS
#define SCLK_1 GpioDataRegs.GPADAT.bit.GPIO4 = 1    //GPIO4作为SCLK
#define SCLK_0 GpioDataRegs.GPADAT.bit.GPIO4 = 0    //GPIO4作为SCLK
#define SDATA_OUT GpioCtrlRegs.GPADIR.bit.GPIO2 = 1 //GPIO2作为SDATA
#define SDATA_IN GpioCtrlRegs.GPADIR.bit.GPIO2 = 0  //GPIO2作为SDATA
#define SDATA GpioDataRegs.GPADAT.bit.GPIO2
#define RST_1 GpioDataRegs.GPADAT.bit.GPIO3 = 1 //GPIO3做RST
#define RST_0 GpioDataRegs.GPADAT.bit.GPIO3 = 0 //GPIO3做RST

unsigned char AC_TABLE[] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, //第一行汉字位置
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, //第二行汉字位置
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, //第三行汉字位置
    0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, //第四行汉字位置
};
static Uint16 frambuff[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int sin[] = {32, 33, 35, 36, 38, 39, 41, 42, 44, 45, 47, 48, 49, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 60, 61, 62, 62, 63, 63, 63, 63, 63, 64, 63, 63, 63, 63, 63, 62, 62, 61, 60, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 49, 48, 47, 45, 44, 42, 41, 39, 38, 36, 35, 33, 32, 30, 28, 27, 25, 24, 22, 21, 19, 18, 16, 15, 14, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16, 18, 19, 21, 22, 24, 25, 27, 28, 30};

#define uchar unsigned char
#define uint unsigned int

extern Uint16 SampleTable[];
int votage_Level = 0;

Uint16 Key = 1;
void put_point(uchar x, uchar y);
int frambufftoXADR(int frambuff);
int frambufftoYADR(int frambuff);

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
{ //输入精确到0.1S,是用","
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
                  data = (data | (0x01 << (7 - i))); //注意！
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
            SDATA = (0x1 & ((x) >> (7 - i))); //先把高位移出然后发送
            SDATA = (0x1 & ((x) >> (7 - i)));
            SCLK_quarter;
            SCLK_1; //把时间改成上升沿锁存的
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
      write_byte(0xFE); //0xFA ==11111110 b 这个代表从MCU到LCD 并且是数据
      tmp1 = read_byte();
      tmp2 = read_byte();
      CS_0;
      execute_72us;
      data = (tmp1 & 0xF0);                 //高四位
      data = (data | ((tmp2 & 0xF0) >> 4)); //低四位
      return data;
}

void lcd_write_data(Uint16 data)
{
      Uint16 tmp1, tmp2;
      CS_1;
      delay_Loop(40);
      tmp1 = (data & 0xF0);        //高四位
      tmp2 = ((data & 0x0F) << 4); //低四位左移
      write_byte(0xFA);            //0xFA ==11111010 b 这个代表从MCU到LCD 并且是数据
      write_byte(tmp1);
      write_byte(tmp2);
      delay_Loop(40);
      CS_0;
      execute_72us;
}

void lcd_write_cmd(Uint16 data) //lcd 命令写
{
      Uint16 tmp1, tmp2;
      CS_1;
      delay_Loop(40);
      tmp1 = (data & 0xF0);        //高四位
      tmp2 = ((data & 0x0F) << 4); //低四位左移
      write_byte(0xF8);            //0xF8 ==11111000 b 这个代表从MCU到LCD 并且是指令
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
      GpioCtrlRegs.GPADIR.all = 0x0000003C;  // GPIO2 3 4 5设置为输出
      GpioCtrlRegs.GPAPUD.all = 0xFFFFFFC3;  // GPIO2 3 4 5设置为内部上拉
      EDIS;
}

/******************************************************************************************
* 函数名称    ：LcmInit
******************************************************************************************/
void LcmInit(void)
{
      lcd_write_cmd(0x30); //8BitMCU,基本指令集合
      lcd_write_cmd(0x03); //AC归0,不改变DDRAM内容
      lcd_write_cmd(0x0C); //显示ON,游标OFF,游标位反白OFF
      lcd_write_cmd(0x01); //清屏,AC归0
      lcd_write_cmd(0x06); //写入时,游标右移动
}

/******************************************************************************************
* 函数名称    ：LcmClearTXT
* 功能描述    ：文本区清RAM函数
******************************************************************************************/
void Lcd_ClearTXT(void)
{
      uchar i;
      lcd_write_cmd(0x30); //8BitMCU,基本指令集合
      lcd_write_cmd(0x80); //AC归起始位
      for (i = 0; i < 64; i++)
            lcd_write_data(0x20);
}

/******************************************************************************************
* 函数名称    ：LcmClearBMP
* 功能描述    ：图形区清RAM函数
******************************************************************************************/
void Lcd_ClearBMP(void)
{
      uchar i, j;
      lcd_write_cmd(0x34);     //8Bit扩充指令集,即使是36H也要写两次
      lcd_write_cmd(0x36);     //绘图ON,基本指令集里面36H不能开绘图
      for (i = 0; i < 32; i++) //12864实际为256x32
      {
            lcd_write_cmd(0x80 | i); //行位置
            lcd_write_cmd(0x80);     //列位置
            for (j = 0; j < 32; j++) //256/8=32 byte
                  lcd_write_data(0);
      }
}
/******************************************************************************************
* 函数名称    ：PutStr
******************************************************************************************/
void lcd_PutStr(uchar row, uchar col, char *puts)
{
      const char endc[] = {"\0"};
      lcd_write_cmd(0x30);                    //8BitMCU,基本指令集合
      lcd_write_cmd(AC_TABLE[8 * row + col]); //起始位置
      strcat(puts, endc);
      while (*puts != '\0') //判断字符串是否显示完毕
      {
            if (col == 8) //判断换行
            {             //若不判断,则自动从第一行到第三行
                  col = 0;
                  row++;
            }
            if (row == 4)
                  row = 0; //一屏显示完,回到屏左上角
            lcd_write_cmd(AC_TABLE[8 * row + col]);
            lcd_write_data(*puts); //一个汉字要写两次
            puts++;
            lcd_write_data(*puts);
            puts++;
            col++;
      }
}

/******************************************************************************************
* 函数名称    ：PutBMP
******************************************************************************************/
void lcd_PutBMP(Uint16 *puts)
{
      uint x = 0;
      uchar i, j;
      lcd_write_cmd(0x34); //8Bit扩充指令集,即使是36H也要写两次
      lcd_write_cmd(0x36); //绘图ON,基本指令集里面36H不能开绘图

      //                if(puts[x]==0x0000)
      //                {
      //                    break;
      //                }

      //      for (i = 0; i < 512; i++) //12864实际为256x32
      //      {
      //          //                if(puts[x]==0x0000)
      //          //                {
      //          //                    break;
      //          //                }
      //            lcd_write_cmd(0x80 | (i/32)); //行位置
      //            lcd_write_cmd(0x80 | (i%32));     //列位置
      //            lcd_write_data(((puts[i] & 0xFF00) >> 8));
      //            lcd_write_data((puts[i] & 0x00FF));
      ////            DELAY_LOOP();
      //
      //      }

      for (i = 0; i < 32; i++) //12864实际为256x32
      {
            lcd_write_cmd(0x80 | i); //行位置
            lcd_write_cmd(0x80);     //列位置
            for (j = 0; j < 16; j++) //256/16=16 byte
            {                        //列位置每行自动增加

                  lcd_write_data(((puts[x] & 0xFF00) >> 8));
                  lcd_write_data((puts[x] & 0x00FF));
                  x++;
            }
      }
}

void lcd_Clean_Screnn_With_Buffer()
{
      uint x = 0;
      uchar i, j;
      // lcd_write_cmd(0x34);     //8Bit扩充指令集,即使是36H也要写两次
      // lcd_write_cmd(0x36);     //绘图ON,基本指令集里面36H不能开绘图
      lcd_ready_to_draw();
      for (i = 0; i < 512; i++)
      {
            if (frambuff[i] != 0x00)
            {

                  lcd_write_cmd(frambufftoYADR(i));
                  lcd_write_cmd(frambufftoXADR(i));
                  //            lcd_write_data(frambuff[i]>>8);
                  //            lcd_write_data(frambuff[i]);
                  lcd_write_data(0x00);
                  lcd_write_data(0x00);
                  frambuff[i]=0x00;
            }
      }
      lcd_draw_over();
      lcd_Clear_frambuff();
      // for (i = 0; i < 32; i++) //12864实际为256x32
      // {
      //       lcd_write_cmd(0x80 | i); //行位置
      //       lcd_write_cmd(0x80);     //列位置
      //       for (j = 0; j < 16; j++) //256/16=16 byte
      //       {                        //列位置每行自动增加
      //           if(puts[x]==0x0000)
      //           {
      //               break;
      //           }
      //             lcd_write_data(((puts[x] & 0x0000) >> 8));
      //             lcd_write_data((puts[x] & 0x0000));
      //             x++;
      //       }
      // }
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
      delay_Loop(632000); //80ms的延时 上电延时
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
      int i = 0;
      lcd_ready_to_draw();
      for (i = 0; i <= 127; i++)
      {
            //lcd_PutPointtoBuffer(i, 13);
            //            lcd_PutPointtoBuffer(i, sin[i]);
            put_point(i, sin[i]);
      }
      lcd_draw_over();
      //      lcd_Update();
}

void lcd_Toast(char *puts)
{
      lcd_PutStr(0, 0, puts);
      int i = 0;
      for (; i < TOAST_TIME; i++)
      {
            delay_Loop(65535);
      }
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
      //    votage_Level=0;
      /****
     * TODO: do add votage_level function here:
     */
      //lcd_Clean_Screnn_With_Buffer(frambuff);
      int j = 0;
      //    for(j=0;j<128;j++){
      //        lcd_PutPointtoBuffer(j,SampleTable[j]>>(6+votage_Level));
      //    }
      //    lcd_Update();
      lcd_ready_to_draw();
      for (j = 0; j < 128; j++)
      {
            put_point(j, SampleTable[j] >> (6 + votage_Level));
      }
      lcd_draw_over();
}

void lcd_ready_to_draw()
{
      lcd_write_cmd(0x34);
      lcd_write_cmd(0x34);
}

void lcd_draw_over()
{
      lcd_write_cmd(0x36); //开显示
      lcd_write_cmd(0x30); //转回基本指令集
}

/**There's no need to calculate x,y to buffer index
*   Change it later
*/
void put_point(uchar x, uchar y)
{
      y=(y>63)?63:y;
      y = 63 - y;
      lcd_PutPointtoBuffer(x, y);
      uint bt = 0;
      uchar x_adr, y_adr, h_bit, l_bit;
      y_adr = 0x80 + y % 32; //计算Y轴的地址，应为纵坐标有64个，所有对32求余，当Y大于31时，Y的坐标是下半屏的。
      if (y > 31)            //计算X轴的地址当Y大于31时X的地址在下半屏，从0X88开始，小于31时X的地址是在上半屏，从0X80开始
            x_adr = 0x88 + x / 16;
      else
            x_adr = 0x80 + x / 16;
      bt = 0x8000 >> (x % 16); //求这个点到底是在哪个点
      int buffrange = xytobuffRange(x, y);
      bt = bt | frambuff[buffrange];
      h_bit = bt >> 8;
      l_bit = bt;
      lcd_write_cmd(y_adr); //写入点的时候，重新写入地址，因为地址已经改变。
      lcd_write_cmd(x_adr);
      lcd_write_data(h_bit);
      lcd_write_data(l_bit);
}
