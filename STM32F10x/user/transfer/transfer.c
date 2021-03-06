#include "transfer.h"
#include "usart/usart.h"
#include "util/util.h"
#include "debug/debug.h"
#include "buffer/buffer.h"
#include <stdio.h>
#include "cJSON.h"

#define MSG_QUEUE_SIZE 50

typedef struct _tagEntity
{
    u8 id;
    u8 to;
    tagRingCache* cache;
    ON_DATA_RECEIVED_CALLBACK cb;
} tagEntity;

typedef u8 msg;

static u8  buffer[MAX_CACHE_SIZE] = { 0 };
static msg mqueue[MSG_QUEUE_SIZE] = { 0 };
static msg* phead = mqueue;
static msg* ptail = mqueue;
static tagEntity entity[] = {
    { USART_COM_INVALID, USART_COM_INVALID, NULL, NULL },
    { USART_COM_INVALID, USART_COM_INVALID, NULL, NULL },
    { USART_COM_INVALID, USART_COM_INVALID, NULL, NULL },
    { USART_COM_INVALID, USART_COM_INVALID, NULL, NULL },
    { USART_COM_INVALID, USART_COM_INVALID, NULL, NULL },
    { USART_COM_INVALID, USART_COM_INVALID, NULL, NULL },
    { USART_COM_INVALID, USART_COM_INVALID, NULL, NULL },
    { USART_COM_INVALID, USART_COM_INVALID, NULL, NULL },
    { USART_COM_INVALID, USART_COM_INVALID, NULL, NULL },
};

void doTask(u8 id)
{
    u32 len = 0;
    tagRingCache* cache = entity[id].cache;
    if (NULL == cache)
    {
        // invalid cache
        return;
    }
    
    len = ReadPacket(cache, buffer, MAX_CACHE_SIZE);
    if (len > 0)
    {
        if (USART_COM_INVALID == entity[id].to)
        {
            //console("%s\r\n", buffer);
            //entity[id].cb(buffer, len);
        }
        else
        {
            entity[entity[id].to].cb(buffer, len);
//             {
//                 u8* text = "{\"abc\": \"debug in\"}";
//                 cJSON* json = cJSON_Parse(text);
//                 u8* pdata = NULL;
//                 u8  jlen = 0;
//                 buffer[len] = '\0';
//                 cJSON_AddStringToObject(json, "recv", buffer);
//                 pdata = cJSON_Print(json);
//                 jlen = digitLength(pdata);
//                 console("%s", pdata);
//                 cJSON_Delete(json);
//             }
        }
    }
    else if (0 > len)
    {
        // do nothing
    }
    else
    {
        // TODO: bug need to be fixed. why program hit it
        // console("failed to read one packet from cache, source: %d", id);
    }

    zero(buffer, MAX_CACHE_SIZE);
}

void Register(tagEUSART tag, tagEUSART target, tagRingCache* cache, ON_DATA_RECEIVED_CALLBACK cb)
{
    ASSERT(tag < MAX_USART_COM_COUNT, "invalid USART tag");
    if (USART_COM_INVALID == entity[tag].id)
    {
        entity[tag].id = tag;
        entity[tag].to = target;
        entity[tag].cache = cache;
        entity[tag].cb = cb;
    }
}

u8 transmit(tagEUSART id)
{
    if (phead + 1 == ptail)
    {
        return 0;
    }

    *phead = id;
    ++phead;

    if (phead > mqueue + MSG_QUEUE_SIZE)
    {
        phead = mqueue;
    }

    return 1;
}

u8 empty(void)
{
    if (ptail != phead)
    {
        return 0;
    }
    return 1;
}

void transfer(void)
{
    doTask(*ptail);
    ++ptail;

    if (ptail > mqueue + MSG_QUEUE_SIZE)
    {
        ptail = mqueue;
    }
}
