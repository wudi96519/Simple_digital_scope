#ifndef __ADCINT_H_
#define __ADCINT_H_

#define BUF_SIZE   256                              //采样的数组长度

extern void adc_Restart();                          //采样完成后ADC自动停止，手动重启，( adc_Over() == 0 )
extern void adc_Init();                             //ADC初始化
extern int  adc_Over();                             //采样完成后返回   1 ，
extern void ADC_config(int config_index);
extern Uint16 SampleTable[];
extern double Voltage,maxVoltage;
extern Uint16 tmpMax;
extern Uint16 tmpMin;
extern Uint16 tmpVal;


#endif
