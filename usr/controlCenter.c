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
int INDEX_TO_DRAW = 0;
int votage_Level = 0;
char msgF[] = "  Freq:   .   Hz \0";
char msgV[] = "vppVolt:  .   V  \0";
char msgMV[] = "maxVolt:  .   V  \0";
void cc_get_trigger_index(int index_num);
void para_test();

extern Uint32 sample_priod;
double freq = 0, period = 0;
extern double Voltage, maxVoltage; //from adcint

void cc_lcd_Draw_Sample(Uint16 *SampleTable);
void cc_lcd_Show_Freq_Volt(double freq, double Voltage, double maxVoltage);
void cc_all_Sys_Init()
{
    InitSysCtrl();
    DINT;
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();
    adc_Init();
    lcd_Init();
    lcd_Toast("   welcome to    digital scope~!\0");
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
    if ((num_Of_index <= 2) && (DISPLAY_CURVE)) //慢速模式
    {
        if (DISPLAY_CURVE)
        {
            static int i = 0;

            if ((sample_index % 16) == 0)
            {
                i++;
                if (i == 9)
                {
                    i = 0;
                }

                {
                    lcd_Clean_Screnn_With_Buffer();
                    lcd_Draw_Curve(SampleTable, 0, 16 * i, (6 + votage_Level));
                    lcd_Draw_With_Buffer();
                }
            }
        }
        if (adc_Over())
        {
            adc_Restart();
        }
    }
    else
    {
        if (adc_Over())
        {
            if (DISPLAY_CURVE)
            {
                cc_lcd_Draw_Sample(SampleTable);
            }
            else if (!DISPLAY_CURVE)
            {
                para_test();
                cc_lcd_Show_Freq_Volt(freq, Voltage, maxVoltage);
            }
            DELAY_LOOP();
            adc_Restart();
        }
    }
}

void cc_get_trigger_index(index_num)
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
}

void ISR_key3()
{
    trigger_flag = 2;
}

void ISR_key4()
{
    num_Of_index = (num_Of_index == 15) ? 15 : num_Of_index + 1;
    ADC_config(num_Of_index);
    sample_index = 0;
}

void ISR_key5()
{
    if (!KEY5PRESSED)
    {
        DISPLAY_CURVE = DISPLAY_CURVE == 1 ? 0 : 1;
        KEY5PRESSED = 1;
    }
}

void ISR_key6()
{
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
}

void ISR_key9()
{
    trigger_flag = 3;
}

void para_test()
{
    double oldFreq = freq;
    cc_get_trigger_index(0);
    cc_get_trigger_index(1);
    period = (double)sample_priod * (double)(index_buff[1] - index_buff[0]) / 150.0 / 256.0;
    freq = 1000000 / period;
    if (freq < 0)
    {
        freq = oldFreq;
    }
}

void cc_lcd_Show_Freq_Volt(double freq, double Voltage, double maxVoltage)
{
    if (freq / 1000 > 1)
    {
        freq /= 1000;
        msgF[13] = 'k';
    }
    else
    {
        msgF[13] = ' ';
    }
    int i = 0;
    msgF[12] = '0' + ((int)(freq * 100)) % 10;
    msgF[11] = '0' + ((int)(freq * 10)) % 10;
    msgF[9] = '0' + ((int)freq) % 10;
    msgF[8] = '0' + ((int)freq) / 10 % 10;
    msgF[7] = '0' + ((int)freq) / 100 % 10;
    for (i = 7; i <= 10; i++)
    {
        if (msgF[i] != '0')
            break;
        else
            msgF[i] = ' ';
    }
    msgV[9] = '0' + (int)Voltage % 10;
    msgV[11] = '0' + (int)(Voltage * 10) % 10;
    msgV[12] = '0' + (int)(Voltage * 100) % 10;
    msgV[13] = '0' + (int)(Voltage * 1000) % 10;

    msgMV[9] = '0' + (int)maxVoltage % 10;
    msgMV[11] = '0' + (int)(maxVoltage * 10) % 10;
    msgMV[12] = '0' + (int)(maxVoltage * 100) % 10;
    msgMV[13] = '0' + (int)(maxVoltage * 1000) % 10;

    lcd_PutStr(0, 0, msgF);
    lcd_PutStr(1, 0, msgV);
    lcd_PutStr(2, 0, msgMV);
}

void cc_lcd_Draw_Sample(Uint16 *SampleTable)
{
    lcd_Clean_Screnn_With_Buffer();
    cc_get_trigger_index(0);
    INDEX_TO_DRAW = index_buff[0];
    lcd_Draw_Curve(SampleTable, INDEX_TO_DRAW, 128, (6 + votage_Level));
    lcd_Draw_With_Buffer();
}
