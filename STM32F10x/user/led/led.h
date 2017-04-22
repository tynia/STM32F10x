#ifndef _STM32_LED_H_
#define _STM32_LED_H_

#include "stm32f10x.h"

typedef enum _tagELED
{
    LED_A = 0,
    LED_B,
    LED_C,
    LED_D,
    LED_E,
    LED_F,
    LED_G,
    LED_COUNT,
} tagELED;

void led_init(tagELED idx, u8 pinx);

void led_on(tagELED idx, u16 pin);

void led_off(tagELED idx, u16 pin);
/*
 * @function: make led2 twinkle
 * @param:
 * @times: twinkle specified times
 * @timespan: the period time of led on and led off. passing value 0 means to use default clock cycle 1000000.
 **/
void led_twinkle(tagELED idx, u16 Pin, u32 times, u32 timespan);

#endif
