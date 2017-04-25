#include "transfer.h"
#include "usart/usart.h"
#include "util/util.h"
#include "debug/debug.h"
#include <stdio.h>

#define TASK_QUEUE_SIZE 50

typedef struct _tagTask
{
    u8 from;
    u8 to;
} tagTask;

typedef struct _tagEntity
{
    u8 id;
    ring_cache* cache;
    ON_DATA_RECEIVED_CALLBACK cb;
} tagEntity;

static u8 buffer[MAX_CACHE_SIZE] = { 0 };
static tagTask  task_queue[TASK_QUEUE_SIZE] = { 0 };
static tagTask* task_head = task_queue;
static tagTask* task_tail = task_queue;
static tagEntity entity[USART_COM_COUNT] = {
    { USART_COM_INVALID, NULL, NULL },
    { USART_COM_INVALID, NULL, NULL },
    { USART_COM_INVALID, NULL, NULL },
    { USART_COM_INVALID, NULL, NULL },
    { USART_COM_INVALID, NULL, NULL },
    { USART_COM_INVALID, NULL, NULL },
    { USART_COM_INVALID, NULL, NULL },
    { USART_COM_INVALID, NULL, NULL },
    { USART_COM_INVALID, NULL, NULL },
};

void doTask(u8 from, u8 to)
{
    u8* ptr = buffer;
    u32 len = 0;
    ring_cache* cache = entity[from].cache;
    if (NULL == cache)
    {
        // invalid cache
        return;
    }
    
    while (read_cache_char(cache, ptr) > 0)
    {
        ++ptr;
        ++len;
    }
    entity[to].cb(buffer, len);

    zero(buffer, MAX_CACHE_SIZE);
}

void reg(u8 idx, ring_cache* cache, ON_DATA_RECEIVED_CALLBACK cb)
{
    ASSERT(idx >= 0 && idx < USART_COM_COUNT, "invalid USART tag");
    if (USART_COM_INVALID == entity[idx].id)
    {
        entity[idx].id = idx;
        entity[idx].cache = cache;
        entity[idx].cb = cb;
    }
}

u8 transmit(u8 from, u8 to)
{
    if (task_head + 1 == task_tail)
    {
        return 0;
    }

    task_head->from = from;
    task_head->to = to;
    ++task_head;

    if (task_head > task_queue + TASK_QUEUE_SIZE)
    {
        task_head = task_queue;
    }

    return 1;
}

void transfer()
{
    while (task_head != task_tail)
    {
        doTask(task_tail->from, task_tail->to);
        ++task_tail;

        if (task_tail > task_queue + TASK_QUEUE_SIZE)
        {
            task_tail = task_queue;
        }
    }
}
