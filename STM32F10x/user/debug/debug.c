#include "debug.h"
#include "util/util.h"

DEBUG_CALLBACK debug_cb = NULL;
DEBUG_CALLBACK set_debug_handler(DEBUG_CALLBACK callback)
{
    DEBUG_CALLBACK old = debug_cb;
    debug_cb = callback;
    return old;
}

void panic()
{
    u8* ptr = NULL;
    console("hit fatal error, pause");
    //__asm int 3;
    *ptr = 233;
}

#define CONSOLE_BUFFER_SIZE 256
static char buffer[CONSOLE_BUFFER_SIZE];
void console(const char* fmt, ...)
{
    if (NULL != debug_cb)
    {
        va_list args;
        u32 len = 0;
        va_start(args, fmt);
        vsnprintf(buffer, CONSOLE_BUFFER_SIZE, fmt, args);
        va_end(args);
        len = str_len(buffer);
        debug_cb(buffer, len);
    }
}
