#include "DSP28x_Project.h" // Device Headerfile and Examples Include File
#include "usr/lcd12864.h"
#include "usr/keyint.h"
#include "usr/adcint.h"
#include "usr/controlCenter.h"

int UP_OR_DOWN_DECIDE_VALUE=1000;
#define BAND 3


int DISPLAY_CURVE=1;
int KEY5PRESSED=0;
Uint16 trigger_flag=0;
int index_buff[2]={0};
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
    {
        lcd_Init();
        lcd_Toast("   welcome to    digital scope~!\0");
    }
    if(EN_KEY)
        keyboard_Scan_Init();
}

void get_trigger_index(index_num)
{
    int i = 0, minus = 0, max = 0, max_flag = 0, zero_flag, count = 0;
    if (trigger_flag == 1) //rising edge
        for (i = 0; i < 256; i++)
        {
            minus = SampleTable[i + 2] - SampleTable[i];
            if (minus - UP_OR_DOWN_DECIDE_VALUE > 0)
            {
                index_buff[count] = i;
                count++;
                if (count > index_num)
                    break;
            }
        }
    else if (trigger_flag == 0) //falling edge
        for (i = 0; i < 256; i++)
        {
            minus = SampleTable[i + 2] - SampleTable[i];
            if (minus + UP_OR_DOWN_DECIDE_VALUE < 0)
            {
                index_buff[count] = i;
                count++;
                if (count > index_num)
                    break;
            }
        }
    else if (trigger_flag == 2) //low value
    {
        zero_flag = 0;
        for (i = 0; i < 256; i++)
        {
            minus = SampleTable[i] / 64;
            if (minus < BAND)
            {
                zero_flag++;
                if (zero_flag == 2)
                {
                    index_buff[count] = i;
                    count++;
                    if (count > index_num)
                        break;
                }
            }
        }
    }
    else if (trigger_flag == 3) //high value
    {
        max = SampleTable[i] / 64;
        for (i = 0; i < 256; i++)
        {
            minus = (int)(SampleTable[i + 1] / 64) - max;
            if (minus > 0 && minus < BAND)
            {
                max = SampleTable[i + 1] / 64;
                max_flag = i;
            }
            else if (minus < 0 && minus + BAND > 0)
            {
                max = max;
            }
        }
        index_buff[count] = max_flag;
        count++;
        if (count < index_num)
            for (i = max_flag + 10; i < 128; i++)
            {
                minus = (int)(SampleTable[i + 1] / 64) - max;
                if (minus > 0 && minus < BAND)
                {
                    max = SampleTable[i + 1] / 64;
                    max_flag = i;
                }
                else if (minus < 0 && minus + BAND > 0)
                {
                    max = max;
                }
            }
        index_buff[count] = max_flag;
    }
}
//
//void get_trigger_index(index_num)
//{
//    int i=0, minus=0,max=0,max_flag=0,zero_flag,count=0;
//    if(index_num==0)
//    {
//        i=0;
//    }
//    if(index_num==1)
//    {
//        i=index_buff[0]+3;
//    }
//    if(trigger_flag==1)//rising edge
//        for(;i<128;i++)
//        {
//            minus = SampleTable[i+2]-SampleTable[i];
//            if(minus>UP_OR_DOWN_DECIDE_VALUE)
//            {
//            	index_buff[count]=i;
//            	i+=3;
//            	count++;
//            	if(count>index_num)
//            		break;
//            }
//        }
//    else if(trigger_flag==0) //falling edge
//        for(;i<128;i++)
//        {
//            minus = SampleTable[i+2]-SampleTable[i];
//            if(minus+UP_OR_DOWN_DECIDE_VALUE<0)
//            {
//            	index_buff[count]=i;
//            	count++;
//                i+=3;
//            	if(count>index_num)
//            		break;
//            }
//        }
//    else if(trigger_flag==2)                 //low value
//    {
//        zero_flag=0;
//        for(;i<128;i++)
//            {
//                minus = SampleTable[i]/64;
//                if(minus<BAND)
//                {
//                    zero_flag++;
//                    if(zero_flag==2)
//                    {
//						index_buff[count]=i;
//						count++;
//						if(count>index_num)
//							break;
//					}
//                }
//            }
//    }
//    else if(trigger_flag==3)                 //high value
//    {
//        max=SampleTable[i]/64;
//        for(;i<128;i++)
//            {
//                minus=(int)(SampleTable[i+1]/64)-max;
//                if(minus>0&&minus<BAND)
//                {
//                    max=SampleTable[i+1]/64;
//                    max_flag=i;
//                }
//                else if(minus<0&&minus+BAND>0)
//                {
//                    max=max;
//                }
//            }
//        index_buff[count]=max_flag;
//        count++;
//        if(count<index_num)
//            for(i=max_flag+10;i<128;i++)
//                {
//                    minus=(int)(SampleTable[i+1]/64)-max;
//                    if(minus>0&&minus<BAND)
//                    {
//                        max=SampleTable[i+1]/64;
//                        max_flag=i;
//                    }
//                    else if(minus<0&&minus+BAND>0)
//                    {
//                        max=max;
//                    }
//                }
//        index_buff[count]=max_flag;
//    }
//}

void ISR_key1()
{
    trigger_flag=0;
}

void ISR_key2()
{
    votage_Level+=votage_Level==5?0:1;
    UP_OR_DOWN_DECIDE_VALUE=1000>> -votage_Level;
//    lcd_Toast("KEY 2\0");
}

void ISR_key3()
{
    trigger_flag=2;
}

void ISR_key4()
{
    //num_Of_index++;
    num_Of_index=(num_Of_index==15)?15:num_Of_index+1;
    ADC_config(num_Of_index);
    sample_index=0;
}

void ISR_key5()
{
//    lcd_Toast("KEY 5\0");
    if(!KEY5PRESSED){
        DISPLAY_CURVE=DISPLAY_CURVE==1?0:1;
        KEY5PRESSED=1;
    }

}

void ISR_key6()
{
   // num_Of_index--;
//    if(num_Of_index>16).
    //.....

    //warnnig
    num_Of_index=(num_Of_index==0)?0:num_Of_index-1;
    ADC_config(num_Of_index);
    sample_index=0;
}

void ISR_key7()
{
    trigger_flag=1;
}

void ISR_key8()
{
    votage_Level-=votage_Level==(-5)?0:1;
    UP_OR_DOWN_DECIDE_VALUE=1000>> -votage_Level;
    //lcd_Toast("KEY 8\0");
}

void ISR_key9()
{
    trigger_flag=3;
//    static char num[2]={'0','\0'};
//    num[0]++;
//    lcd_Toast(num);
////    lcd_Toast("KEY 9\0");
}
