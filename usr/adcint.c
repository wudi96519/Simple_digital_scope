#include "DSP28x_Project.h" // Device Headerfile and Examples Include File
#include "adcint.h"

#if (CPU_FRQ_150MHZ)   // Default - 150 MHz SYSCLKOUT
#define ADC_MODCLK 0x3 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*3)   = 25.0 MHz
#endif
#if (CPU_FRQ_100MHZ)
#define ADC_MODCLK 0x2 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 100/(2*2)   = 25.0 MHz
#endif
#define ADC_CKPS 0x0  // ADC module clock = HSPCLK/2*ADC_CKPS   = 25.0MHz/(1*2) = 12.5MHz
#define ADC_SHCLK 0x0 // S/H width in ADC module periods                        = 16 ADC clocks

Uint16 SampleTable[BUF_SIZE] = {0};
Uint16 num_Of_Avg[] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1};
Uint16 num_Of_config1[] = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
Uint16 num_Of_config2[] = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
Uint16 num_To_Over[] = {1000, 500, 200, 100, 50, 20, 10, 5, 2, 1, 0, 0, 0, 0, 0, 0};

Uint16 num_Of_index = 15,
       conversion_count = 1,
       sample_index = 0,
       over_count = 0;
Uint32 avg_sum = 0, sample_priod = 0;
Uint16 adc_finish_flag = 0, adc_Pause_flag = 0, adc_Paused_flag = 0;

double Voltage = 3.0, maxVoltage = 3.0;
Uint16 tmpMax = 0, tmpMin = 4095, tmpVal = 0;

void ADC_config(int config_index);
void adc_Go_On();
__interrupt void adc_isr(void);
void adc_Init()
{
    EALLOW;
    SysCtrlRegs.HISPCP.all = ADC_MODCLK; // HSPCLK = SYSCLKOUT/ADC_MODCLK
    PieVectTable.ADCINT = &adc_isr;
    EDIS;
    InitCpuTimers(); // For this example, only initialize the Cpu Timers
#if (CPU_FRQ_150MHZ)
    ConfigCpuTimer(&CpuTimer1, 150, 20000000);
#endif
    CpuTimer1Regs.TCR.all = 0x4010;
    InitAdc();
    AdcRegs.ADCMAXCONV.all = 0x0000;
    AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;
    AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;
    AdcRegs.ADCTRL1.bit.CPS = 0;
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 0; // 1  Cascaded mode
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;
    AdcRegs.ADCTRL1.bit.CONT_RUN = 1; // Setup continuous run
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;
    PieCtrlRegs.PIEIER1.bit.INTx6 = 1;
    PieCtrlRegs.PIEIER12.bit.INTx1 = 1; // Enable PIE Gropu 1 INT4
    PieCtrlRegs.PIEIER12.bit.INTx2 = 1; // Enable PIE Gropu 1 INT5
    PieCtrlRegs.PIEIER12.bit.INTx3 = 1;
    IER |= M_INT12;
    IER |= M_INT1;
    EINT; // Enable Global interrupt INTM
    ERTM;
    AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;
    CpuTimer1Regs.TCR.bit.TSS = 0;
}

void ADC_config(int config_index)
{
    AdcRegs.ADCMAXCONV.all = 0x0000;
    AdcRegs.ADCTRL1.bit.ACQ_PS = num_Of_config1[config_index];
    AdcRegs.ADCTRL3.bit.ADCCLKPS = num_Of_config2[config_index];
    AdcRegs.ADCTRL1.bit.CPS = 0;
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 0; // 1  Cascaded mode
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;
    AdcRegs.ADCTRL1.bit.CONT_RUN = 1; // Setup continuous run
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;
    AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;
    CpuTimer1Regs.TCR.bit.TSS = 0;
}

void adc_Go_On()
{
    if (adc_Pause_flag == 0)
    {
        AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
        PieCtrlRegs.PIEACK.all |= PIEACK_GROUP1;
    }
}

void adc_Pause()
{
    adc_Pause_flag = 1;
}

void adc_Non_Pause()
{
    adc_Pause_flag = 0;
}

void adc_Restart()
{
    tmpMax = 0;
    tmpMin = 4095;
    adc_Non_Pause();
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP1;
    adc_finish_flag = 0;
    CpuTimer1.RegsAddr->TCR.bit.TSS = 0;
}

int adc_Over()
{
    Voltage = (double)(tmpMax - tmpMin) / 4095.0 * 3.0;
    maxVoltage = (double)tmpMax / 4095.0 * 3.0;
    return adc_finish_flag;
}

__interrupt void adc_isr(void)
{
    if (over_count >= num_To_Over[num_Of_index])
    {
        avg_sum += (AdcRegs.ADCRESULT0 >> 4);
        over_count = 0;
        if (conversion_count >= num_Of_Avg[num_Of_index])
        {
            conversion_count = 1;
            tmpVal = avg_sum / num_Of_Avg[num_Of_index];
            SampleTable[sample_index] = tmpVal;
            if (tmpVal > tmpMax)
            {
                tmpMax = tmpVal;
            }
            else if (tmpVal < tmpMin)
            {
                tmpMin = tmpVal;
            }
            sample_index++;
            avg_sum = 0;
        }
        else
        {
            conversion_count++;
        }

        if (sample_index >= BUF_SIZE)
        {
            sample_index = 0;
            CpuTimer1.RegsAddr->TCR.bit.TSS = 1;
            sample_priod = CpuTimer1.RegsAddr->PRD.all - CpuTimer1.RegsAddr->TIM.all;
            CpuTimer1.RegsAddr->TCR.bit.TRB = 1;
            adc_finish_flag = 1;
        }
        else
        {
            adc_Go_On();
        }
    }
    else
    {
        over_count++;
        adc_Go_On();
    }
}
