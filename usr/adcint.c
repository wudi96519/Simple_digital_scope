#include "DSP28x_Project.h" // Device Headerfile and Examples Include File
#include "adcint.h"

#if (CPU_FRQ_150MHZ)     // Default - 150 MHz SYSCLKOUT
#define ADC_MODCLK 0x3 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*3)   = 25.0 MHz
#endif
#if (CPU_FRQ_100MHZ)
#define ADC_MODCLK 0x2 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 100/(2*2)   = 25.0 MHz
#endif
#define ADC_CKPS   0x0   // ADC module clock = HSPCLK/2*ADC_CKPS   = 25.0MHz/(1*2) = 12.5MHz
#define ADC_SHCLK  0x4   // S/H width in ADC module periods                        = 16 ADC clocks
#define AVG        1000  // Average sample limit
#define ZOFFSET    0x00  // Average Zero offset

Uint16 SampleTable[BUF_SIZE]={0},avg_group[1000]={0};
Uint16 num_Of_Avg[] ={1000,500,200,100,50,20,10,500,200,100,50,20,10,5,2,1};
Uint16 num_To_Over[]={99,99,99,99,99,99,99,0,0,0,0,0,0,0,0,0};
Uint16 num_Of_index=15,//档位标志
        conversion_count=1,//一个平均周期内的转换计数
        sample_index=0,//数据最终数组指针
        over_count=0;//省略采样计数
Uint32 avg_sum=0,sample_priod=0;//平均求和
Uint16 adc_finish_flag=0;

__interrupt void adc_isr(void);
void adc_Init()
{
    EALLOW;
    SysCtrlRegs.HISPCP.all = ADC_MODCLK;    // HSPCLK = SYSCLKOUT/ADC_MODCLK
    PieVectTable.ADCINT = &adc_isr;
    EDIS;
    InitCpuTimers();   // For this example, only initialize the Cpu Timers
 #if (CPU_FRQ_150MHZ)
    ConfigCpuTimer(&CpuTimer1, 150, 20000000);
 #endif
    CpuTimer1Regs.TCR.all = 0x4010;
    InitAdc();
    AdcRegs.ADCMAXCONV.all = 0x0000;
    AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;
    AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;        // 1  Cascaded mode
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;
    AdcRegs.ADCTRL1.bit.CONT_RUN = 1;       // Setup continuous run
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;
    PieCtrlRegs.PIEIER1.bit.INTx6 = 1;
	PieCtrlRegs.PIEIER12.bit.INTx1 = 1;          // Enable PIE Gropu 1 INT4
	PieCtrlRegs.PIEIER12.bit.INTx2 = 1;          // Enable PIE Gropu 1 INT5
	PieCtrlRegs.PIEIER12.bit.INTx3 = 1;
	IER |= M_INT12;
	IER |= M_INT1;
	EINT;          // Enable Global interrupt INTM
    ERTM;
    AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;
    CpuTimer1Regs.TCR.bit.TSS=0;
}

void adc_Restart()
{
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;PieCtrlRegs.PIEACK.all |= PIEACK_GROUP1;
    adc_finish_flag=0;
}

int adc_Over()
{
    return adc_finish_flag;
}

__interrupt void adc_isr(void)
{
	if(over_count>=num_To_Over[num_Of_index])
	{
		avg_sum +=(AdcRegs.ADCRESULT0>>4);
		over_count=0;
		if(conversion_count>=num_Of_Avg[num_Of_index])
		{

			conversion_count=1;
			SampleTable[sample_index]=avg_sum/num_Of_Avg[num_Of_index];
			sample_index++;
			avg_sum=0;
		}
		else
		{conversion_count++;}

		if(sample_index>=BUF_SIZE)//采集完一次，可设置一个flag用于显示
		{
			sample_index=0;
			sample_priod=CpuTimer1.RegsAddr->TIM.all;
			CpuTimer1.RegsAddr->TCR.bit.TRB = 1;
			adc_finish_flag=1;
		}
		else
		{AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;PieCtrlRegs.PIEACK.all |= PIEACK_GROUP1;}

	}
	else
		{over_count++;AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;PieCtrlRegs.PIEACK.all |= PIEACK_GROUP1; }
}
