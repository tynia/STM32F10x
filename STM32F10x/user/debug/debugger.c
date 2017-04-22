#include "debugger.h"
#include "debug.h"
#include "buffer/buffer.h"
#include "usart/usart.h"
#include "stm32f10x_usart.h"

static u8 buffer[MAX_CACHE_SIZE];
static ring_cache* cache;
static u8 debugger = USART_COM_INVALID;
extern u8 acceptor = USART_COM_INVALID;

//////////////////////////////////////////////////////////////////////
void debug_irq_handler(void)
{
    u8 r;
    if (0 != usart_recv_data(debugger, &r))
    {
        write_cache_char(cache, r);
    }

    if (0 != usart_is_ok(debugger, USART_IT_IDLE))
    {
        debug("received msg, processing");
        // TODO:
        // read cache and handle
        // if (USART_COM_INVALID != acceptor)
        // {
        //     usart_send_data(acceptor);
        // }
        debug("receive msg processed over");
    }
}

void debug_handler(u8* data, u32 len)
{
    if (USART_COM_INVALID != debugger)
    {
        usart_send_data(debugger, data, len);
    }
}

void debug_init(u8 idx, u16* irq)
{
    ASSERT((idx >= 0 && idx < USART_COM_COUNT), "invalid usart index");

    usart_init(idx, irq, 3, 3, debug_irq_handler);
#ifdef _DEBUG
    ring_cache_init("DEBUG", cache, buffer, MAX_CACHE_SIZE);
#else
    ring_cache_init(cache, buffer, MAX_CACHE_SIZE);
#endif
    debugger = idx;
    set_debug_handler(debug_handler);
}

void set_usart_acceptor(u8 idx)
{
    acceptor = idx;
}