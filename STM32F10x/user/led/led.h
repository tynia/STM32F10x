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
    LED_END,
} tagELED;

void InitLED(tagELED tag, u8 xPin);

void LEDOn(tagELED tag, u16 xPin);

void LEDOff(tagELED tag, u16 xPin);

u8 IsLEDOn(tagELED tag, u16 xPin);

void LEDTwinkle(tagELED tag, u16 xPin, u32 times, u32 timespan);

#endif
