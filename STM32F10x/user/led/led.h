#ifndef _STM32_LED_H_
#define _STM32_LED_H_

#include "stm32f10x.h"

void led_init(u8 idx, u8 pinx);

void led_on(u8 idx, u16 Pin, u32 timespan);

void led_off(u8 idx, u16 Pin, u32 timespan);
/*
 * @function: make led2 twinkle
 * @param:
 * @times: twinkle specified times
 * @timespan: the period time of led on and led off. passing value 0 means to use default clock cycle 1000000.
 **/
void led_twinkle(u8 idx, u16 Pin, u32 times, u32 timespan);

#endif
