#include "DSP28x_Project.h"
#include "usr/lcd12864.h"
#include "usr/keyint.h"
#include "usr/adcint.h"
#include "usr/controlCenter.h"
extern int index_buff[2];
extern void get_trigger_index(int index_num);
extern Uint32 sample_priod;
void para_test();

extern Uint32 sample_priod;
double freq=0,period=0;
extern double Voltage;

void main(void)
{
    all_Sys_Init();
    Lcd_ClearBMP();
     for(;;){
         if(KEY5PRESSED)
         {
             lcd_Clean_Screnn_With_Buffer();
             Lcd_ClearTXT();
             KEY5PRESSED=0;
         }
         if(DISPLAY_CURVE){
             if(adc_Over()){
                 //Lcd_ClearTXT();
                 //lcd_Draw_Sample();
                 if(EN_DEBUG)
                 {
                     lcd_Sin_test();
                     DELAY_LOOP();
                 }
                 else
                 {
                     lcd_Draw_Sample();
                     DELAY_LOOP();
                 }
 //                DELAY_LOOP();
                 adc_Restart();
             }
         }
         if(!DISPLAY_CURVE)
                    {
                        para_test();
                        lcd_Show_Freq_Volt(freq, Voltage);
                        adc_Restart();
                    }
     }
}

void para_test()
{
    get_trigger_index(2);
    period=(double)sample_priod*(double)(index_buff[1]-index_buff[0])/150.0/256.0;
    freq=1000000/period;
}
