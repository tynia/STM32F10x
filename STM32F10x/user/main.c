#include "led/led.h"
#include "gpio/gpio.h"
#include "debug/debugger.h"
#include "usart/usart.h"
#include "at/aithinker_a6.h"
#include "util/util.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

int main(void)
{
    u16 irq[2];
    //SystemInit();
    irq[0] = USART_IT_IDLE;
    irq[1] = USART_IT_RXNE;
    // init debugger
    debugger_init(USART_COM_4, irq);
    // init led
    led_init(GPIO_A, GPIO_Pin_2);
    // init a6
    a6_init(USART_COM_3, irq);
    
    while (1)
    {
        led_twinkle(LED_A, GPIO_Pin_2, 5, 0);
    }
    
    return 0;
}
