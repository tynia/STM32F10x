#include "frame.h"
#include <stdio.h>
#include "cJSON.h"

void fetch(u8** data, u8* out, u8 len)
{
    u8 i = 0;

    if (NULL == data || NULL == *data || NULL == out)
    {
        return;
    }

    while (i < len)
    {
        *(out + i) = *((*data) + 1);
        ++i;
    }
    *data += len;
}

void append(u8** data, u8* in, u8 len)
{
    u8 i = 0;

    if (NULL == data || NULL == *data || NULL == in)
    {
        return;
    }

    while (i < len)
    {
        *((*data) + i) = *(in + i);
        ++i;
    }
    *data += len;
}

state sanity_check(u8 lhs, u8 rhs)
{
    if (lhs != rhs)
    {
        return SFALSE;
    }
    return STRUE;
}

s8 parse(u8* data, frame* out)
{
    frame_header* header = NULL;
    cJSON* json = NULL;
    if (NULL == data || NULL == out)
    {
        // illegal parameter
        return -1;
    }

    header = (frame_header*)data;

    // sync code sanity check
    if (SFALSE == sanity_check(header->sync.value.low, 0xEA) || SFALSE == sanity_check(header->sync.value.high, 0xEF))
    {
        // not a valid sync code
        return -1;
    }

    out->header.sync = header->sync;
    out->header.msgLen = header->msgLen;
    out->header.padding = header->padding;
    out->header.device = header->device;
    out->header.msgid = header->msgid;
    out->header.type = header->type;

    json = cJSON_Parse((const char*)(data + FRAME_HEADER_LEN));
    if (NULL != json)
    {
        out->pdata = (u8*)cJSON_PrintUnformatted(json);
    }

    return 0;
}

void serialize(frame* in, u8* out)
{
    frame_header* header = NULL;
    if (NULL == in || NULL == out)
    {
        // illegal parameter
        return;
    }

    header = &(in->header);

    append(&out, &(header->sync.value.low), 1);
    append(&out, &(header->sync.value.high), 1);
    append(&out, &(header->msgLen), 1);
    append(&out, &(header->padding), 1);
    append(&out, (u8*)&(header->device), sizeof(u32));
    append(&out, (u8*)&(header->msgid), sizeof(u32));
    append(&out, (u8*)&(header->msgLen), sizeof(u16));
    append(&out, in->pdata, header->msgLen - FRAME_HEADER_LEN);
}
