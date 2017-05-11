#ifndef _IOT_FRAME_H_
#define _IOT_FRAME_H_

#include "stm32f10x.h"

typedef enum _enum_state {
    SFALSE = 0,
    STRUE = !SFALSE
} state;

typedef union _union_word {
    struct {
        u8 low;
        u8 high;
    } value;
    u16 entire;
} word;

typedef struct _tag_frame_header
{
    word sync;                          // 1st  - 2nd  byte
    u8  msgLen;                         // 3rd         byte
    u8  padding;                        // 4th         byte
    u32 device;                         // 5th  - 8th  byte
    u32 msgid;                          // 9th  - 12   byte
    u16 type;                           // 13th - 14th byte
} frame_header;

#define FRAME_HEADER_LEN 14

typedef struct _tag_frame {
    frame_header header;
    u8* pdata;
} frame;

void fetch(u8** data, u8* out, u8 len);
void append(u8** data, u8* in, u8 len);
state sanity_check(u8 lhs, u8 rhs);
s8 parse(u8* data, frame* out);
void serialize(frame* in, u8* out);

#endif // !_IOT_FRAME_H_
