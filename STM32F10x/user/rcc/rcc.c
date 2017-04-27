#include "rcc.h"

void InitAPB1CLKCTRL(u32 APB, FunctionalState state)
{
    RCC_APB1PeriphClockCmd(APB, state);
}

void InitAPB2CLKCTRL(u32 APB, FunctionalState state)
{
    RCC_APB2PeriphClockCmd(APB, state);
}
