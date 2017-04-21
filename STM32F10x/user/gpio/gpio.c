#include "gpio.h"
#include "util/util.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

typedef struct _tagGPIOGroup
{
    u32 GPIOPeriph;
    GPIO_InitTypeDef* GPIOx;
} tagGPIOGroup;

static tagGPIOGroup GPIOGroup[GPIO_COUNT] = {
    { RCC_APB2Periph_GPIOA, GPIOA },
    { RCC_APB2Periph_GPIOB, GPIOB },
    { RCC_APB2Periph_GPIOC, GPIOC },
    { RCC_APB2Periph_GPIOD, GPIOD },
    { RCC_APB2Periph_GPIOE, GPIOE },
    { RCC_APB2Periph_GPIOF, GPIOF },
    { RCC_APB2Periph_GPIOG, GPIOG }
};

void GPIOInit(tagEGPIO EGPIO, u16 Pinx, u8 Mode, u8 Speed)
{
    ASSERT((EGPIO >= 0 && EGPIO < GPIO_COUNT), "invalid GPIO tag");
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = Pinx;
    GPIO_InitStruct.GPIO_Mode = Mode;
    GPIO_InitStruct.GPIO_Speed = Speed;
    GPIO_Init(GPIOGroup[EGPIO].GPIOx, &GPIO_InitStruct);
}

void GPIOSetBits(tagEGPIO EGPIO, u16 Pin, u8 on)
{
    ASSERT((EGPIO >= 0 && EGPIO < GPIO_COUNT), "invalid GPIO tag");

    if (0 != on)
    {
        GPIO_SetBits(GPIOGroup[EGPIO].GPIOx, Pin);
    }
    else
    {
        GPIO_ResetBits(GPIOGroup[EGPIO].GPIOx, Pin);
    }
}