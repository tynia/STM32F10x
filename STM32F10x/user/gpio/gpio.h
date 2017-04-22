#ifndef _STM32_GPIO_H_
#define _STM32_GPIO_H_

#include "stm32f10x.h"

typedef enum _tagEGPIO
{
    GPIO_A = 0,
    GPIO_B,
    GPIO_C,
    GPIO_D,
    GPIO_E,
    GPIO_F,
    GPIO_G,
    GPIO_COUNT,
} tagEGPIO;

void gpio_init(tagEGPIO idx, u16 pinx, u8 mode, u8 speed);

void gpio_set_bits(tagEGPIO idx, u16 pin, u8 on);

#endif
