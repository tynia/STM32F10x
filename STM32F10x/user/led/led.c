#include "stm32f10x_gpio.h"
#include "led.h"
#include "gpio/gpio.h"
#include "debug/debug.h"
#include "util/util.h"

#define MAX_LED_COUNT LED_END
#define ENABLE(x) (state & (1 << x))
static u8 state = 0;

void InitLED(tagELED tag, u8 xPin)
{
    ASSERT((tag < LED_END), "invalid LED tag");
    InitGPIOCTRL(tag, xPin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    state |= (1 << tag);
    LEDOff(tag, xPin);
}

void LEDOn(tagELED tag, u16 xPin)
{
    ASSERT((tag < LED_END), "invalid LED tag");
    LLevel(tag, xPin);
}

void LEDOff(tagELED tag, u16 xPin)
{
    ASSERT((tag < LED_END), "invalid LED tag");
    HLevel(tag, xPin);
}

void twinkle(tagELED tag, u16 xPin, u32 timespan)
{
    ASSERT((tag < LED_END), "invalid LED tag");
    if (ENABLE(tag))
    {
        LEDOn(tag, xPin);
        wait(timespan);
        LEDOff(tag, xPin);
        wait(timespan);
    }
}

void LEDTwinkle(tagELED tag, u16 xPin, u32 times, u32 timespan)
{
    u32 i = 0;
    ASSERT((tag < LED_END), "invalid LED tag");
    while (i < times)
    {
        twinkle(tag, xPin, timespan);
        ++i;
    }
}
