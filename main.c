#include "DSP28x_Project.h"
#include "usr/controlCenter.h"

void main(void)
{
    cc_all_Sys_Init();
    for (;;)
    {
        cc_main_loop();
    }
}
