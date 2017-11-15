#ifndef __ADCINT_H_
#define __ADCINT_H_

/**
 * TODO:
 *      ��lcd�Ĳ������ܷ����ڰ���ʱ����ʱADC��һ���رգ�������Ҫ��ͣ adc �ж�
 *      wudi����wenhaoд adc_STOP();
 */

#define BUF_SIZE   256                              //���������鳤��

extern void adc_Restart();                          //������ɺ�ADC�Զ�ֹͣ���ֶ�������( adc_Over() == 0 )
extern void adc_Init();                             //ADC��ʼ��
extern int  adc_Over();                             //������ɺ󷵻�   1 ��

static Uint16 SampleTable[];

#endif
