#include "led.h"

void LEDInit(tagGPIO* GPIO)
{
    GPIOInit(GPIO);
}

void LEDOn(tagGPIO* GPIO, u16 Pinx, u32 timespan)
{
    ASSERT(NULL != GPIO, "invalid GPIO for LED");
    GPIO_SetBits(GPIO->GPIOx, Pinx);
    wait(timespan);
}

void LEDOff(tagGPIO* GPIO, u16 Pinx, u32 timespan)
{
    ASSERT(NULL != GPIO, "invalid GPIO for LED");
    GPIO_SetBits(GPIO->GPIOx, Pinx);
    wait(timespan);
}

void twinkle(tagGPIO* GPIO, u16 Pinx, u32 timespan)
{
    ASSERT(NULL != GPIO, "invalid GPIO for LED");
    LEDOn(GPIO, Pinx, timespan);
    LEDOff(GPIO, Pinx, timespan);
}

void LEDTwinkle(tagGPIO* GPIO, u16 Pinx, u32 times, u32 timespan)
{
    ASSERT(NULL != GPIO, "invalid GPIO for LED");
    u32 i = 0;
    while (i < times)
    {
        twinkle(GPIO, Pinx, timespan);
        ++i;
    }
}
