#include "debug.h"
#include "util/util.h"

DEBUG_CALLBACK cb = NULL;
DEBUG_CALLBACK SetDebugOutHandler(DEBUG_CALLBACK callback)
{
    DEBUG_CALLBACK old = cb;
    cb = callback;
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
    if (NULL != cb)
    {
        va_list args;
        u32 len = 0;
        va_start(args, fmt);
        vsnprintf(buffer, CONSOLE_BUFFER_SIZE, fmt, args);
        va_end(args);
        len = digitLength((u8*)buffer);

        // append 0x0D 0x0A
        buffer[len] = '\r';
        buffer[len + 1] = '\n';
        buffer[len + 2] = '\0';
        len += 3;
        if (NULL != cb)
        {
            cb((u8*)buffer, len);
        }
    }
}
