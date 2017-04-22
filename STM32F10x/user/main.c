#include "led/led.h"
#include "gpio/gpio.h"
#include "stm32f10x_gpio.h"
#include "usart/usart.h"
#include "stm32f10x_usart.h"

int main(void)
{
    u16 irq[3];
    irq[0] = USART_IT_TC;
    irq[1] = USART_IT_RXNE;
    irq[2] = USART_IT_IDLE;
    led_init(GPIO_A, GPIO_Pin_2);
    a6_init(USART_COM_3, irq);
    debugger_init(USART_COM_4, irq);
   return 0;
}
