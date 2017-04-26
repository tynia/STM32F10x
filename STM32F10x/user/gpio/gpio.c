#include "gpio.h"
#include "debug/debug.h"
#include "rcc/rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

typedef struct _tagGPIO
{
    u32 APBGPIOx;
    GPIO_TypeDef* GPIOx;
} tagGPIO;

#define MAX_GPIO_COUNT xGPIO_END

static tagGPIO GPIOGroup[MAX_GPIO_COUNT] = {
    { RCC_APB2Periph_GPIOA, GPIOA },
    { RCC_APB2Periph_GPIOB, GPIOB },
    { RCC_APB2Periph_GPIOC, GPIOC },
    { RCC_APB2Periph_GPIOD, GPIOD },
    { RCC_APB2Periph_GPIOE, GPIOE },
    { RCC_APB2Periph_GPIOF, GPIOF },
    { RCC_APB2Periph_GPIOG, GPIOG }
};

void InitGPIOCTRL(tagEGPIO xGPIO, u16 xPin, u8 Mode, u8 Speed)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ASSERT(xGPIO < xGPIO_END, "invalid GPIO index");
    InitAPBCLKCTRL(GPIOGroup[xGPIO].APBGPIOx, ENABLE);
    GPIO_InitStructure.GPIO_Pin = xPin;
    GPIO_InitStructure.GPIO_Mode = (GPIOMode_TypeDef)Mode;
    GPIO_InitStructure.GPIO_Speed = (GPIOSpeed_TypeDef)Speed;
    GPIO_Init(GPIOGroup[xGPIO].GPIOx, &GPIO_InitStructure);
}

void Enable(tagEGPIO xGPIO, u16 xPin)
{
    ASSERT(xGPIO < xGPIO_END, "invalid GPIO index");
    GPIO_ResetBits(GPIOGroup[xGPIO].GPIOx, xGPIO);
}

void Disable(tagEGPIO xGPIO, u16 xPin)
{
    ASSERT(xGPIO < xGPIO_END, "invalid GPIO index");
    GPIO_SetBits(GPIOGroup[xGPIO].GPIOx, xGPIO);
}
