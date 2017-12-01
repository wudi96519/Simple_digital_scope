#include "DSP28x_Project.h"
#include "lcd12864.h"

static Uint16 ROW = 0, COLUMN = 0;
Uint16 KEY = 0;
extern void ISR_key1();
extern void ISR_key2();
extern void ISR_key3();
extern void ISR_key4();
extern void ISR_key5();
extern void ISR_key6();
extern void ISR_key7();
extern void ISR_key8();
extern void ISR_key9();

/**
 * 
 * 鑳戒繚璇佷竴娆＄煭鏃堕棿鎸変笅锛岃櫧鐒跺彲鑳借Е鍙戝娆′腑鏂紝浣嗘槸鍙墽琛屼竴娆℃浠跺搴斿嚱鏁�
 * 瀵逛簬闀挎椂闂存寜涓嬶紝闂撮殧涓�瀹氭椂闂存墽琛屼竴娆�
 **/
void function_Select()
{
    switch (KEY)
    {
    case 1:
        ISR_key1();
        KEY=0;
        break;
    case 2:
        ISR_key2();
        KEY=0;
        break;
    case 3:
        ISR_key3();
        KEY=0;
        break;
    case 4:
        ISR_key4();
        KEY=0;
        break;
    case 5:
        ISR_key5();
        KEY=0;
        break;
    case 6:
        ISR_key6();
        KEY=0;
        break;
    case 7:
        ISR_key7();
        KEY=0;
        break;
    case 8:
        ISR_key8();
        KEY=0;
        break;
    case 9:
        ISR_key9();
        KEY=0;
        break;
    default:
        break;
    }
}

void scan_ready()
{
    ROW=0;
    GpioDataRegs.GPBCLEAR.bit.GPIO48 = 1;
    GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;
    GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1;
}

void key_scan()
{
    Uint16 i = 0;
    COLUMN = 0;
    GpioDataRegs.GPBSET.bit.GPIO48 = 1;
    GpioDataRegs.GPBSET.bit.GPIO49 = 1;
    GpioDataRegs.GPBSET.bit.GPIO50 = 1;
    DELAY_US(1);

    for (i = 0; i < 5; i++)
    {
        GpioDataRegs.GPBCLEAR.bit.GPIO48 = 1;
        DELAY_US(1);
        if (GpioDataRegs.GPBDAT.bit.GPIO53 == 0 || GpioDataRegs.GPBDAT.bit.GPIO52 == 0 || GpioDataRegs.GPBDAT.bit.GPIO51 == 0)
        {
            COLUMN = 3;
            KEY = (ROW - 1) * 3 + COLUMN;
            break;
        }

        GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;
        DELAY_US(1);
        if (GpioDataRegs.GPBDAT.bit.GPIO53 == 0 || GpioDataRegs.GPBDAT.bit.GPIO52 == 0 || GpioDataRegs.GPBDAT.bit.GPIO51 == 0)
        {
            COLUMN = 2;
            KEY = (ROW - 1) * 3 + COLUMN;
            break;
        }

        GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1;
        DELAY_US(1);
        if (GpioDataRegs.GPBDAT.bit.GPIO53 == 0 || GpioDataRegs.GPBDAT.bit.GPIO52 == 0 || GpioDataRegs.GPBDAT.bit.GPIO51 == 0)
        {
            COLUMN = 1;
            KEY = (ROW - 1) * 3 + COLUMN;
            break;
        }
    }
    scan_ready();
}

void key_Xint_Function()
{
    key_scan();
    function_Select();
    DELAY_LOOP();
    DELAY_LOOP();
    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP12;
}

__interrupt void row1_Isr(void)
{
    ROW = 1;
    key_Xint_Function();
}

__interrupt void row2_Isr(void)
{
    ROW = 2;
    key_Xint_Function();
}

__interrupt void row3_Isr(void)
{
    ROW = 3;
    key_Xint_Function();
}


void xint_init()
{
    // GPIO30 & GPIO31 are outputs, start GPIO30 high and GPIO31 low
    EALLOW;
    GpioDataRegs.GPBSET.bit.GPIO48 = 1;  // Load the output latch
    GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 0; // GPIO
    GpioCtrlRegs.GPBDIR.bit.GPIO48 = 1;  // output

    GpioDataRegs.GPBSET.bit.GPIO49 = 1;  // Load the output latch
    GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0; // GPIO
    GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;  // output

    GpioDataRegs.GPBSET.bit.GPIO50 = 1;  // Load the output latch
    GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 0; // GPIO
    GpioCtrlRegs.GPBDIR.bit.GPIO50 = 1;  // output



    GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 0;  // GPIO
    GpioCtrlRegs.GPBDIR.bit.GPIO51 = 0;   // input
    GpioCtrlRegs.GPBQSEL2.bit.GPIO51 = 0; // XINT3 Qual using 6 samples

    GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 0;  // GPIO
    GpioCtrlRegs.GPBDIR.bit.GPIO52 = 0;   // input
    GpioCtrlRegs.GPBQSEL2.bit.GPIO52 = 0; // XINT3 Qual using 6 samples

    GpioCtrlRegs.GPBMUX2.bit.GPIO53 = 0;      // GPIO
    GpioCtrlRegs.GPBDIR.bit.GPIO53 = 0;       // input
    GpioCtrlRegs.GPBQSEL2.bit.GPIO53 = 0;     // XINT3 Qual using 6 samples
    GpioCtrlRegs.GPBCTRL.bit.QUALPRD2 = 0x48; // Each sampling window is 150*SYSCLKOUT



    GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL = 53; // Xint3 is GPIO0
    GpioIntRegs.GPIOXINT4SEL.bit.GPIOSEL = 52; // XINT4 is GPIO1
    GpioIntRegs.GPIOXINT5SEL.bit.GPIOSEL = 51; // XINT5 is GPIO1


    XIntruptRegs.XINT3CR.bit.POLARITY = 0; // Falling edge interrupt
    XIntruptRegs.XINT4CR.bit.POLARITY = 0; // Rising edge interrupt
    XIntruptRegs.XINT5CR.bit.POLARITY = 0;


    XIntruptRegs.XINT3CR.bit.ENABLE = 1; // Enable Xint1
    XIntruptRegs.XINT4CR.bit.ENABLE = 1; // Enable XINT2
    XIntruptRegs.XINT5CR.bit.ENABLE = 1;
    EDIS;
}

void keyboard_Scan_Init()
{
    EALLOW; // This is needed to write to EALLOW protected register
            //      PieVectTable.ADCINT = &adc_isr;
    PieVectTable.XINT3 = &row1_Isr;
    PieVectTable.XINT4 = &row2_Isr;
    PieVectTable.XINT5 = &row3_Isr;
    EDIS;
    PieCtrlRegs.PIEIER1.bit.INTx6 = 1;
    PieCtrlRegs.PIEIER12.bit.INTx1 = 1; // Enable PIE Gropu 1 INT4
    PieCtrlRegs.PIEIER12.bit.INTx2 = 1; // Enable PIE Gropu 1 INT5
    PieCtrlRegs.PIEIER12.bit.INTx3 = 1;
    IER |= M_INT12;
    IER |= M_INT1;
    EINT; // Enable Global interrupt INTM
    ERTM;
    xint_init();
    scan_ready();
}
