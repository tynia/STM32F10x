#include "led/led.h"
#include "gpio/gpio.h"
#include "transfer/transfer.h"
#include "acceptor/acceptor.h"
#include "usart/usart.h"
#include "at/aithinker_a6.h"
#include "util/util.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

int main(void)
{
    u16 irq[3];
    irq[0] = USART_IT_RXNE;
#ifdef _DEBUG
    // init debugger
    debugger_init(USART_COM_3, USART_COM_4);
#else
    acceptor_init(USART_COM_3, irq, 3, USART_COM_4);
#endif
    // init a6
    a6_init(USART_COM_4, irq, 1, USART_COM_3);
    // init led
    led_init(GPIO_A, GPIO_Pin_2);

    while (1)
    {
        transfer();
        wait(5000);
        debug_in();
    }
    
    return 0;
}
