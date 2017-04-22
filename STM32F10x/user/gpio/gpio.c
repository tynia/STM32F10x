#include "gpio.h"
#include "util/util.h"
#include "debug/debug.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

typedef struct _tag_gpio_group
{
    u32 gpio_periph;
    GPIO_TypeDef* gpiox;
} tag_gpio_group;

static tag_gpio_group GPIOGroup[GPIO_COUNT] = {
    { RCC_APB2Periph_GPIOA, GPIOA },
    { RCC_APB2Periph_GPIOB, GPIOB },
    { RCC_APB2Periph_GPIOC, GPIOC },
    { RCC_APB2Periph_GPIOD, GPIOD },
    { RCC_APB2Periph_GPIOE, GPIOE },
    { RCC_APB2Periph_GPIOF, GPIOF },
    { RCC_APB2Periph_GPIOG, GPIOG }
};

void gpio_init(u8 idx, u16 pinx, u8 mode, u8 speed)
{
    GPIO_InitTypeDef gpio_init_struct;
    ASSERT((idx >= 0 && idx < GPIO_COUNT), "invalid GPIO idx");
    RCC_APB2PeriphClockCmd(GPIOGroup[idx].gpio_periph, ENABLE);
    gpio_init_struct.GPIO_Pin = pinx;
    gpio_init_struct.GPIO_Mode = (GPIOMode_TypeDef)mode;
    gpio_init_struct.GPIO_Speed = (GPIOSpeed_TypeDef)speed;
    GPIO_Init(GPIOGroup[idx].gpiox, &gpio_init_struct);
}

void gpio_set_bits(u8 idx, u16 pin, u8 on)
{
    ASSERT((idx >= 0 && idx < GPIO_COUNT), "invalid GPIO idx");

    if (0 != on)
    {
        GPIO_SetBits(GPIOGroup[idx].gpiox, pin);
    }
    else
    {
        GPIO_ResetBits(GPIOGroup[idx].gpiox, pin);
    }
}
