#include "led.h"
#include "gpio/gpio.h"
#include "debug/debug.h"
#include "util/util.h"
#include "stm32f10x_gpio.h"

enum
{
    LED_ON = 0,
    LED_OFF = !LED_ON,
};
static u8 LED_TABLE[LED_COUNT] = { 0 };

void led_init(tagELED idx, u8 pinx)
{
    ASSERT((idx < LED_COUNT && idx >= LED_A), "invalid LED index");
    gpio_init(idx, pinx, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    LED_TABLE[idx] = 1;
    gpio_set_bits(idx, pinx, 1);
}

void led_on(tagELED idx, u16 pinx)
{
    ASSERT((idx < LED_COUNT && idx >= LED_A), "invalid LED index");
    gpio_set_bits(idx, pinx, LED_ON);
}

void led_off(tagELED idx, u16 pinx)
{
    ASSERT((idx < LED_COUNT && idx >= LED_A), "invalid LED index");
    gpio_set_bits(idx, pinx, LED_OFF);
}

void twinkle(tagELED idx, u16 pinx, u32 timespan)
{
    ASSERT((idx < LED_COUNT && idx >= LED_A), "invalid LED index");
    if (0 != LED_TABLE[idx])
    {
        led_on(idx, pinx);
        wait(timespan);
        led_off(idx, pinx);
        wait(timespan);
    }
}

void led_twinkle(tagELED idx, u16 pinx, u32 times, u32 timespan)
{
    u32 i = 0;
    ASSERT((idx < LED_COUNT && idx >= LED_A), "invalid LED index");
    while (i < times)
    {
        twinkle(idx, pinx, timespan);
        ++i;
    }
}
