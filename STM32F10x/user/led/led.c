#include "led.h"
#include "gpio/gpio.h"
#include "debug/debug.h"
#include "util/util.h"
#include "stm32f10x_gpio.h"

static u8 LED_TABLE[LED_COUNT] = { 0 };

void led_init(u8 idx, u8 pinx)
{
    ASSERT((idx < LED_COUNT && idx >= LED_A), "invalid LED idx for LED");
    gpio_init(idx, pinx, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    LED_TABLE[idx] = 1;
    gpio_set_bits(idx, pinx, 1);
}

void led_on(u8 idx, u16 pinx, u32 timespan)
{
    ASSERT((idx < LED_COUNT && idx >= LED_A), "invalid LED idx for LED");
    gpio_set_bits(idx, pinx, 0);
    wait(timespan);
}

void led_off(u8 idx, u16 pinx, u32 timespan)
{
    ASSERT((idx < LED_COUNT && idx >= LED_A), "invalid LED idx for LED");
    gpio_set_bits(idx, pinx, 1);
    wait(timespan);
}

void twinkle(u8 idx, u16 pinx, u32 timespan)
{
    ASSERT((idx < LED_COUNT && idx >= LED_A), "invalid LED idx for LED");
    if (0 != LED_TABLE[idx])
    {
        led_on(idx, pinx, timespan);
        led_off(idx, pinx, timespan);
    }
}

void led_twinkle(u8 idx, u16 pinx, u32 times, u32 timespan)
{
    u32 i = 0;
    ASSERT((idx < LED_COUNT && idx >= LED_A), "invalid LED idx for LED");
    while (i < times)
    {
        twinkle(idx, pinx, timespan);
        ++i;
    }
}
