#include "debugger.h"
#include "debug.h"
#include "stm32f10x_usart.h"
#include "usart/usart.h"
#include "buffer/buffer.h"

DEBUG_CALLBACK debug_cb = NULL;
DATA_RECEIVED_CALLBACK data_recv_cb = NULL;
static u8 debugger = USART_INVALID;
static ring_cache debugcache;
static ring_cache* cache = &debugcache;
static u8 buffer[MAX_CACHE_SIZE];
static u8 rbuffer[MAX_CACHE_SIZE] = { 0 };
static u8 ro = 0;

void debug_data_handler(u8* data, u32 len)
{
    if (USART_INVALID != debugger)
    {
        usart_send_data(debugger, data, len);
    }
}

void debug_data_received()
{
    u8* ptr = rbuffer;
    u32 len = 0;
    ASSERT(NULL != cache, "invalid cache");
    while (read_cache_char(cache, ptr) > 0)
    {
        ++ptr;
        ++len;
    }
    data_recv_cb(rbuffer, len);
}

void debugger_irq_handler(void)
{
    u8 r;
    if (0 != usart_recv_data(debugger, &r))
    {
        if (r == 0x0D)
        {
            ro = 1;
        }
        else if (r == 0x0A)
        {
            ro = 0;
        }
        else
        {
            write_cache_char(cache, &r);
        }
    }
}

///////////////////////////////////////////////////////
void debugger_init(u8 idx)
{
    u16 irq[3];
    irq[0] = USART_IT_IDLE;
    irq[1] = USART_IT_RXNE;
    irq[2] = USART_IT_TC;
    usart_init(idx, 3, 3, irq, 3, NULL);
    ring_cache_init(cache, buffer, MAX_CACHE_SIZE);
    debugger = idx;
    set_debug_data_handler(debug_data_handler);
}

DATA_RECEIVED_CALLBACK set_data_received_handler(DATA_RECEIVED_CALLBACK func)
{
    DATA_RECEIVED_CALLBACK old = data_recv_cb;
    data_recv_cb = func;
    return old;
}
