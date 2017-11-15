//###########################################################################
// LCD12864——dsp28335 数字简易示波器
//###########################################################################

#include "DSP28x_Project.h" // Device Headerfile and Examples Include File
#include "usr/lcd12864.h"
#include "usr/keyint.h"
#include "usr/adcint.h"
#include "usr/controlCenter.h"

/***
 * TODO:  * 频率测量、幅值测量
 */

/******************************************************************************************/


void main(void)
{
    all_Sys_Init();
    Lcd_ClearBMP();
     for(;;){
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
                        lcd_PutStr(0, 0,"Freq:\0");
                        DELAY_LOOP();
                        Lcd_ClearTXT();
                        Lcd_ClearBMP();
                    }

     }
}
