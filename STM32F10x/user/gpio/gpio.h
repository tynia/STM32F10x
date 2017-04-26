#ifndef _STM32_GPIO_H_
#define _STM32_GPIO_H_

#include "stm32f10x.h"
#include "gpio/gpio.h"

typedef enum _tagEGPIO
{
    AGPIO = 0,
    BGPIO,
    CGPIO,
    DGPIO,
    EGPIO,
    FGPIO,
    GGPIO,
    xGPIO_END,
} tagEGPIO;

void InitGPIOCTRL(tagEGPIO xGPIO, u16 xPin, u8 Mode, u8 Speed);

void Enable(tagEGPIO xGPIO, u16 xPin);

void Disable(tagEGPIO xGPIO, u16 xPin);

#endif
