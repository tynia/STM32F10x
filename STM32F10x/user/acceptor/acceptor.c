#include "acceptor.h"
#include "debug/debug.h"
#include "buffer/buffer.h"
#include "usart/usart.h"
#include "led/led.h"
#include "util/util.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

static u8 acceptor = USART_COM_INVALID;
static u8 target   = USART_COM_INVALID;
static u8 buffer[MAX_CACHE_SIZE];
static ring_cache dummy;
static ring_cache* cache = &dummy;
u8 recv_state = 0;

void exchange_irq_handler(void)
{
    u8 r;
    if (0 != usart_recv_data(acceptor, &r))
    {
        write_cache_char(cache, &r);
        if (r == 0x0D)
        {
            recv_state = 1;
        }
        else if (r == 0x0A)
        {
            recv_state = 0;
            transmit(acceptor, target);
        }
    }
}

void accepter_data_handler(u8* data, u32 len)
{
    if (USART_COM_INVALID != acceptor)
    {
        usart_send_data(acceptor, data, len);
    }
}

///////////////////////////////////////////////////////
void acceptor_init(u8 idx, u16* irq, u8 len, u8 acceptor_target)
{
    ASSERT((idx >= 0 && idx < USART_COM_COUNT), "invalid usart index");
    ring_cache_init(cache, buffer, MAX_CACHE_SIZE);
    reg(idx, cache, accepter_data_handler);
    acceptor = idx;
    target = acceptor_target;
    usart_init(idx, 3, irq, len, exchange_irq_handler);
}
