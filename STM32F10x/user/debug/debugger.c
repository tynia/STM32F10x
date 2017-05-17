#include "debugger.h"
#include "debug.h"
#include "usart/usart.h"
#include "buffer/buffer.h"
#include "led/led.h"
#include "transfer/transfer.h"
#include "stm32f10x_usart.h"
#include "cJSON.h"
#include "buffer/frame.h"
#include "stm32f10x_gpio.h"
#include "util/util.h"

static tagEUSART debugger = USART_COM_INVALID;
static u8 buffer[MAX_CACHE_SIZE];
static tagRingCache* cache = NULL;
static u16 toRead = 0;
#define CHECK(x, y)   (x |= (1 << y))
#define ISCHECK(x, y) (x & (1 << y))
static u8 syncState = 0;
// byte   1 1 1 1 1 1 1 1
// bit    7 6 5 4 3 2 1 0
//        |     |     |
//    recv len  |     |
//          recv 0xEF |
//                recv 0xEA
#define RECV_SYNC_1 1
#define RECV_SYNC_2 4
#define RECV_LENGTH 7

// print out through debug com
void OnDebugData(u8* data, u32 len)
{
    //debug
    if (0xEA == *data)
    {
        frame frame_in;
        frame frame_out;
        u8 buf[256] = { 0 };
        cJSON* json = NULL;
        cJSON* item = NULL;
        parse(data, &frame_in);
        json = cJSON_Parse((const char*)(frame_in.pdata));
        if (NULL != json)
        {
            item = cJSON_GetObjectItem(json, "method");
            if (NULL != item)
            {
                if (item->valueint == 0)
                {
                    if (IsLEDOn(LED_B, GPIO_Pin_5) != 0)
                    //if (IsLEDOn(LED_A, GPIO_Pin_2) != 0)
                    {
                        frame_out.pdata = "{\"result\":0,\"on_off\":0}";
                    }
                    else
                    {
                        frame_out.pdata = "{\"result\":0,\"on_off\":1}";
                    }
                }
                else if (item->valueint == 1)
                {
                    cJSON* on = cJSON_GetObjectItem(json, "on_off");
                    if (NULL != on)
                    {
                        if (on->valueint == 1)
                        {
                            LEDOn(LED_B, GPIO_Pin_5);
                            //LEDOn(LED_A, GPIO_Pin_2);
                            frame_out.pdata = "{\"result\":0,\"on_off\":1}";
                        }
                        else
                        {
                            LEDOff(LED_B, GPIO_Pin_5);
                            //LEDOff(LED_A, GPIO_Pin_2);
                            frame_out.pdata = "{\"result\":0,\"on_off\":0}";
                        }
                    }
                }
            }
            cJSON_Delete(json);
        }
        else
        {
            frame_out.pdata = "{\"result\":1, \"msg\":\"invalid msg\"}";
        }
        frame_out.header.sync = frame_in.header.sync;
        frame_out.header.msgLen = 14 + digitLength(frame_out.pdata);
        frame_out.header.padding = 0;
        frame_out.header.device = frame_in.header.device;
        frame_out.header.msgid = frame_in.header.msgid;
        frame_out.header.type = frame_in.header.type;

        serialize(&frame_out, buf);
        USARTSendData(debugger, buf, 14 + digitLength(frame_out.pdata));
        return;
    }

    if (USART_COM_INVALID != debugger)
    {
        USARTSendData(debugger, data, len);
    }
}

void SendDebugFrameData(u8 c)
{
    u32 msgid  = 10010;
    u32 device = 10020;
    u16 type = 10030;
    u8* json = NULL;
    if (c == 'o')
    {
        json = "{\"method\":1, \"on_off\":1}";
    }
    else if (c == 'f')
    {
        json = "{\"method\":1, \"on_off\":0}";
    }
    else if (c == 's')
    {
        json = "{\"method\":0}";
    }
    WriteChar(cache, 0xEA);
    WriteChar(cache, 0xEF);
    WriteChar(cache, 14+digitLength(json));
    WriteChar(cache, 0);
    Write(cache, (u8*)&device, sizeof(u32));
    Write(cache, (u8*)&msgid, sizeof(u32));
    Write(cache, (u8*)&type, sizeof(u16));
    Write(cache, json, digitLength(json));
}

void DebuggerIRQHandler(void)
{
    u8 r = 0;
    if (0 != USARTRecvData(debugger, &r))
    {
        // debug
//         if ('f' == r || 'o' == r || 's' == r)
//         {
// 			//console(&r);
//             SendDebugFrameData(r);
//             transmit(debugger);
//             return;
//         }

        if (0 != ISCHECK(syncState, RECV_SYNC_2))
        {
            if (0 == ISCHECK(syncState, RECV_LENGTH))
            {
                CHECK(syncState, RECV_LENGTH);
                toRead = r - 2;
            }
            WriteChar(cache, r);
            --toRead;
            if (toRead == 0)
            {
                syncState = 0;
                transmit(debugger);
            }
        }
        else if (0 != ISCHECK(syncState, RECV_SYNC_1))
        {
            if (r == 0xEF)
            {
                CHECK(syncState, 4);
                WriteChar(cache, 0xEA);
                WriteChar(cache, 0xEF);
            }
            else
            {
                syncState = 0;
            }
        }
        else
        {
            if (r == 0xEA)
            {
                CHECK(syncState, RECV_SYNC_1);
            }
        }
    }
}

void InitDebugger(tagEUSART tag, tagEUSART target)
{
    u16 irq;
    irq = USART_IT_RXNE;
    cache = InitRingCache(buffer, MAX_CACHE_SIZE);
    ASSERT(NULL != cache, "OOM, failed to init cache");
    Register(tag, target, cache, OnDebugData);
    debugger = tag;
    SetDebugOutHandler(OnDebugData);
    InitUSARTCTRL(tag, 1, 0, &irq, 1, DebuggerIRQHandler);
}
