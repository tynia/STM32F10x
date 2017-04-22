#include "debug.h"

#define TRACE_BUFFER_SIZE 256
#define DEBUG_BUFFER_SIZE 512

DEBUG_CALLBACK cb = NULL;

void set_debug_handler(DEBUG_CALLBACK callback)
{
    cb = callback;
}
/////////////////////////////////////////////////
#ifdef _DEBUG
void panic()
{
    __asm int 3;
}
#endif

static u8 debug_buffer[DEBUG_BUFFER_SIZE];
static u8 trace_buffer[TRACE_BUFFER_SIZE];
static u8* traceInfo = "[File:%32s Line:%d Func:%32s] Msg:\r\n%s\r\n";
void debug(u8* fmt, ...)
{
    if (NULL != cb)
    {
        va_list args;
        va_start(args, fmt);
        vsnprintf(debug_buffer, DEBUG_BUFFER_SIZE, fmt, args);
        va_end(args);
        u32 len = str_len(debug_buffer);
        cb(debug_buffer, len);
    }
}

void wrap_trace(u8* file, u32 line, u8* func, u8* fmt, ...)
{
    if (NULL != cb)
    {
        zero(trace_buffer, TRACE_BUFFER_SIZE);
        va_list args;
        va_start(args, fmt);
        vsnprintf(trace_buffer, TRACE_BUFFER_SIZE, fmt, args);
        va_end(args);

        debug(traceInfo, file, line, func, trace_buffer);
    }
}