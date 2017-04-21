#include "led.h"
#include "gpio/gpio.h"
#include "stm32f10x_gpio.h"

void led_init(u8 idx, u8 pinx)
{
    ASSERT((idx < GPIO_COUNT && idx >= GPIO_A), "invalid GPIO idx for LED");
    gpio_init(idx, pinx, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
}

void led_on(u8 idx, u16 pinx, u32 timespan)
{
    ASSERT((idx < GPIO_COUNT && idx >= GPIO_A), "invalid GPIO idx for LED");
    gpio_set_bits(idx, pinx, 1);
    wait(timespan);
}

void led_off(u8 idx, u16 pinx, u32 timespan)
{
    ASSERT((idx < GPIO_COUNT && idx >= GPIO_A), "invalid GPIO idx for LED");
    GPIO_SetBits(idx, pinx, 0);
    wait(timespan);
}

void twinkle(u8 idx, u16 pinx, u32 timespan)
{
    ASSERT((idx < GPIO_COUNT && idx >= GPIO_A), "invalid GPIO idx for LED");
    led_on(idx, pinx, timespan);
    led_off(idx, pinx, timespan);
}

void led_twinkle(u8 idx, u16 pinx, u32 times, u32 timespan)
{
    ASSERT((idx < GPIO_COUNT && idx >= GPIO_A), "invalid GPIO idx for LED");
    u32 i = 0;
    while (i < times)
    {
        twinkle(idx, pinx, timespan);
        ++i;
    }
}
