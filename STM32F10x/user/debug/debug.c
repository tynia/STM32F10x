#include "debug.h"
#include "util/util.h"

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

static char debug_buffer[DEBUG_BUFFER_SIZE];
static char trace_buffer[TRACE_BUFFER_SIZE];
static u8* traceInfo = "[File:%32s Line:%d Func:%32s] Msg:\r\n%s\r\n";
void debug(const char* fmt, ...)
{
    if (NULL != cb)
    {
        va_list args;
        u32 len = 0;
        va_start(args, fmt);
        vsnprintf(debug_buffer, DEBUG_BUFFER_SIZE, fmt, args);
        va_end(args);
        len = str_len(debug_buffer);
#ifdef _DEBUG
        cb(debug_buffer, len);
#endif
    }
}

void debugc(const char* fmt, ...)
{
    if (NULL != cb)
    {
        va_list args;
        u32 len = 0;
        va_start(args, fmt);
        vsnprintf(debug_buffer, DEBUG_BUFFER_SIZE, fmt, args);
        va_end(args);
        len = str_len(debug_buffer);
        cb(debug_buffer, len);
    }
}

void wrap_trace(const char* file, u32 line, const char* func, const char* fmt, ...)
{
    if (NULL != cb)
    {
        va_list args;
        zero(trace_buffer, TRACE_BUFFER_SIZE);
        va_start(args, fmt);
        vsnprintf(trace_buffer, TRACE_BUFFER_SIZE, fmt, args);
        va_end(args);

        debug(traceInfo, file, line, func, trace_buffer);
    }
}
