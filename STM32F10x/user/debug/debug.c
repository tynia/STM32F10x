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
#define LR "\r\n"
#define MARKENTER(fmt) #fmt##LR
void console(const char* fmt, ...)
{
    if (NULL != cb)
    {
        va_list args;
        u32 len = 0;
        va_start(args, fmt);
        vsnprintf(buffer, CONSOLE_BUFFER_SIZE, MARKENTER(fmt), args);
        va_end(args);
        len = digitLength((u8*)buffer);
        cb((u8*)buffer, len);
    }
}
