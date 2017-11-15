#ifndef __ADCINT_H_
#define __ADCINT_H_

#define BUF_SIZE   256                              //采样的数组长度

extern void adc_Restart();                          //采样完成后ADC自动停止，手动重启，( adc_Over() == 0 )
extern void adc_Init();                             //ADC初始化
extern int  adc_Over();                             //采样完成后返回   1 ，

extern Uint16 SampleTable[];

#endif
