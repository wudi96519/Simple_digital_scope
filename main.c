//###########################################################################
// LCD12864——dsp28335 数字简易示波器
//###########################################################################

#include "DSP28x_Project.h" // Device Headerfile and Examples Include File
#include "usr/lcd12864.h"
#include "usr/keyint.h"
#include "usr/adcint.h"
#include "usr/controlCenter.h"

/***
 * TODO: 时间尺度  &&  电压尺度  &&  触发方式
 *       频率测量  ==> 控制中心
 */

/******************************************************************************************/


void main(void)
{
    all_Sys_Init();
    lcd_Toast("    欢迎使用      数字示波器~!\0");
    //lcd_Sin_test();
    for(;;){
        if(DISPLAY_CURVE){
            if(adc_Over()){
                Lcd_ClearTXT();
                lcd_Draw_Sample();
//                DELAY_LOOP();
                adc_Restart();
            }
        }
        if(!DISPLAY_CURVE)
                   {
                       Lcd_ClearTXT();
                       Lcd_ClearBMP();
                       lcd_PutStr(0, 0, "频率:\0");
                       DELAY_LOOP();
                   }

    }
}
