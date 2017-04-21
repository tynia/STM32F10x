#include "debug.h"
#include "buffer/buffer.h"
#include "usart/usart.h"
#include "stm32f10x_usart.h"

static u8 buffer[MAX_CACHE_SIZE];
static ring_cache* cache;
static u8 DEBUGLOG = USART_COM_INVALID;

//////////////////////////////////////////////////////////////////////
void debug_irq_handler(void)
{
    u8 r;
    if (0 != usart_recv_data(DEBUGLOG, &r))
    {
        write_cache_char(cache, r);
    }

    if (0 != usart_is_ok(DEBUGLOG, USART_IT_IDLE))
    {
        debug("")
        curCommand->state = AT_STATE_RECV;
    }
}

void Debug(u8 idx, u16* irq)
{
    ASSERT((idx >= 0 && idx < USART_COM_COUNT), "invalid usart index");

    usart_init(idx, irq, 3, 3, debug_irq_handler);
#ifdef _DEBUG
    ring_cache_init("DEBUG", cache, buffer, MAX_CACHE_SIZE);
#else
    ring_cache_init(cache, buffer, MAX_CACHE_SIZE);
#endif
    DEBUGLOG = idx;
}

/////////////////////////////////////////////////
#ifdef _DEBUG
void panic()
{
    __asm int 3;
}
#endif

void debug(u8* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsprintf(fmt, args);
#ifdef _DEBUG
    // TODO:
#else
    // TODO:
#endif
    va_end(args);
}

void trace(u8* file, u32 line, u8* func, u8* fmt, ...)
{
    static u8* traceInfo = "File: %-32s Line: %d\r\nFunc: %-32s Msg:\r\n%s\r\n";
    va_list args;
    va_start(args, fmt);
    vsprintf(fmt, args);
    va_end(args);

    // TODO:
    // write to TX pin for log

}