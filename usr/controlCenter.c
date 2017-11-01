#include "DSP28x_Project.h" // Device Headerfile and Examples Include File
#include "usr/lcd12864.h"
#include "usr/keyint.h"
#include "usr/adcint.h"
#include "usr/controlCenter.h"
/**
 * TODO:给每个按键分配功能
 *      设定适当全局变量
 */

int DISPLAY_CURVE=1;
void all_Sys_Init()
{
    InitSysCtrl();
    DINT;
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();
    if(EN_ADC)
        adc_Init();
    if(EN_LCD)
        lcd_Init();
    if(EN_KEY)
        keyboard_Scan_Init();
}


void ISR_key1()
{
    lcd_Toast("KEY 1\0");
}

void ISR_key2()
{
    votage_Level+=votage_Level==5?0:1;
//    lcd_Toast("KEY 2\0");
}

void ISR_key3()
{
    lcd_Toast("KEY 3\0");
}

void ISR_key4()
{
    //num_Of_index++;
    num_Of_index=(num_Of_index==15)?15:num_Of_index+1;
    sample_index=0;
}

void ISR_key5()
{
//    lcd_Toast("KEY 5\0");
    DISPLAY_CURVE=DISPLAY_CURVE==1?0:1;
    if(!DISPLAY_CURVE)
            {
                Lcd_ClearTXT();
                Lcd_ClearBMP();
                lcd_PutStr(0, 0, "频率:\0");
                DELAY_LOOP();
            }
}

void ISR_key6()
{
   // num_Of_index--;
//    if(num_Of_index>16).
    //.....

    //warnnig
    num_Of_index=(num_Of_index==0)?0:num_Of_index-1;
    sample_index=0;
}

void ISR_key7()
{
    lcd_Toast("KEY 7\0");
}

void ISR_key8()
{
    votage_Level-=votage_Level==(-5)?0:1;
    //lcd_Toast("KEY 8\0");
}

void ISR_key9()
{
    static char num[2]={'0','\0'};
    num[0]++;
    lcd_Toast(num);
//    lcd_Toast("KEY 9\0");
}
