#include "gpio.h"
#include "util/util.h"
#include "debug/debug.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

typedef struct _tagGPIO
{
    u32 GPIO_Periph;
    GPIO_TypeDef* GPIOx;
} tagGPIO;

static tagGPIO GPIOGroup[GPIO_COUNT] = {
    { RCC_APB2Periph_GPIOA, GPIOA },
    { RCC_APB2Periph_GPIOB, GPIOB },
    { RCC_APB2Periph_GPIOC, GPIOC },
    { RCC_APB2Periph_GPIOD, GPIOD },
    { RCC_APB2Periph_GPIOE, GPIOE },
    { RCC_APB2Periph_GPIOF, GPIOF },
    { RCC_APB2Periph_GPIOG, GPIOG }
};

void gpio_init(tagEGPIO idx, u16 pinx, u8 mode, u8 speed)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    ASSERT((idx >= 0 && idx < GPIO_COUNT), "invalid GPIO index");
    RCC_APB2PeriphClockCmd(GPIOGroup[idx].GPIO_Periph, ENABLE);
    GPIO_InitStruct.GPIO_Pin = pinx;
    GPIO_InitStruct.GPIO_Mode = (GPIOMode_TypeDef)mode;
    GPIO_InitStruct.GPIO_Speed = (GPIOSpeed_TypeDef)speed;
    GPIO_Init(GPIOGroup[idx].GPIOx, &GPIO_InitStruct);
}

void gpio_set_bits(tagEGPIO idx, u16 pin, u8 on)
{
    ASSERT((idx >= 0 && idx < GPIO_COUNT), "invalid GPIO index");

    if (0 != on)
    {
        GPIO_SetBits(GPIOGroup[idx].GPIOx, pin);
    }
    else
    {
        GPIO_ResetBits(GPIOGroup[idx].GPIOx, pin);
    }
}
