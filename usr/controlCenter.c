#include "DSP28x_Project.h" // Device Headerfile and Examples Include File
#include "usr/lcd12864.h"
#include "usr/keyint.h"
#include "usr/adcint.h"
#include "usr/controlCenter.h"

int UP_OR_DOWN_DECIDE_VALUE = 1000;
#define BAND 5

int DISPLAY_CURVE = 1;
int KEY5PRESSED = 0;
Uint16 trigger_flag = 0;
int index_buff[2] = {0};
void get_trigger_index(int index_num);
void para_test();

extern Uint32 sample_priod;
double freq = 0, period = 0;
extern double Voltage;

void cc_all_Sys_Init()
{
    InitSysCtrl();
    DINT;
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();
    if (EN_ADC)
        adc_Init();
    if (EN_LCD)
    {
        lcd_Init();
        lcd_Toast("   welcome to    digital scope~!\0");
    }
    if (EN_KEY)
        keyboard_Scan_Init();
}

void cc_main_loop()
{
    if (KEY5PRESSED)
    {
        lcd_Clean_Screnn_With_Buffer();
        Lcd_ClearTXT();
        KEY5PRESSED = 0;
    }
    if (adc_Over())
    {
        if (DISPLAY_CURVE)
        {
            if (EN_DEBUG_LCD)
            {
                lcd_Sin_test();
            }
            else
            {
                lcd_Draw_Sample(SampleTable);
            }
        }
        else if (!DISPLAY_CURVE)
        {
            para_test();
            lcd_Show_Freq_Volt(freq, Voltage);
        }
        DELAY_LOOP();
        adc_Restart();
    }
}

void get_trigger_index(index_num)
{
    UP_OR_DOWN_DECIDE_VALUE = (int)(0.6 * (tmpMax - tmpMin));
    index_buff[index_num] = 0;
    int i = 0, minus = 0;
    if (index_num == 1)
    {
        i = index_buff[0] + 3;
    }
    if (trigger_flag == 0) //falling edge
        while (i < 128 * (1 + index_num))
        {
            minus = SampleTable[i + 2] - SampleTable[i];
            if (minus + UP_OR_DOWN_DECIDE_VALUE < 0)
            {
                index_buff[index_num] = i + 1;
                break;
            }
            i++;
        }
    else if (trigger_flag == 1) //rising edge
        while (i < 128 * (1 + index_num))
        {
            minus = SampleTable[i + 2] - SampleTable[i];
            if (minus > UP_OR_DOWN_DECIDE_VALUE)
            {
                index_buff[index_num] = i + 1;
                break;
            }
            i++;
        }
    else if (trigger_flag == 2) //low value
    {
        while (i < 128 * (1 + index_num))
        {
            if (SampleTable[i] <= tmpMin + BAND)
            {
                index_buff[index_num] = i;
                break;
            }
            i++;
        }
    }
    else if (trigger_flag == 3) //high value
    {
        while (i < 128 * (1 + index_num))
        {
            if (SampleTable[i] >= tmpMax - BAND)
            {
                index_buff[index_num] = i;
                break;
            }
            i++;
        }
    }
}

void ISR_key1()
{
    trigger_flag = 0;
}

void ISR_key2()
{
    votage_Level += votage_Level == 5 ? 0 : 1;
    UP_OR_DOWN_DECIDE_VALUE = 1000 >> -votage_Level;
    //    lcd_Toast("KEY 2\0");
}

void ISR_key3()
{
    trigger_flag = 2;
}

void ISR_key4()
{
    //num_Of_index++;
    num_Of_index = (num_Of_index == 15) ? 15 : num_Of_index + 1;
    ADC_config(num_Of_index);
    sample_index = 0;
}

void ISR_key5()
{
    //    lcd_Toast("KEY 5\0");
    if (!KEY5PRESSED)
    {
        DISPLAY_CURVE = DISPLAY_CURVE == 1 ? 0 : 1;
        KEY5PRESSED = 1;
    }
}

void ISR_key6()
{
    // num_Of_index--;
    //    if(num_Of_index>16).
    //.....

    //warnnig
    num_Of_index = (num_Of_index == 0) ? 0 : num_Of_index - 1;
    ADC_config(num_Of_index);
    sample_index = 0;
}

void ISR_key7()
{
    trigger_flag = 1;
}

void ISR_key8()
{
    votage_Level -= votage_Level == (-5) ? 0 : 1;
    UP_OR_DOWN_DECIDE_VALUE = 1000 >> -votage_Level;
    //lcd_Toast("KEY 8\0");
}

void ISR_key9()
{
    trigger_flag = 3;
    //    static char num[2]={'0','\0'};
    //    num[0]++;
    //    lcd_Toast(num);
    ////    lcd_Toast("KEY 9\0");
}

void para_test()
{
    double oldFreq = freq;
    get_trigger_index(0);
    get_trigger_index(1);
    period = (double)sample_priod * (double)(index_buff[1] - index_buff[0]) / 150.0 / 256.0;
    freq = 1000000 / period;
    if (freq < 0)
    {
        freq = oldFreq;
    }
}
