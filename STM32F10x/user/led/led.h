#ifndef _STM32_LED_H_
#define _STM32_LED_H_

#include "gpio/gpio.h"

void LEDInit(tagGPIO* GPIO);

void LEDOn(tagGPIO* GPIO, u16 Pin, u32 timespan);

void LEDOff(tagGPIO* GPIO, u16 Pin, u32 timespan);
/*
 * @function: make led2 twinkle
 * @param:
 * @times: twinkle specified times
 * @timespan: the period time of led on and led off. passing value 0 means to use default clock cycle 1000000.
 **/
void LEDTwinkle(tagGPIO* GPIO, u16 Pin, u32 times, u32 timespan);

#endif
