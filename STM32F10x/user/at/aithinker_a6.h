#ifndef _STM32_AITHINKER_A6_H_
#define _STM32_AITHINKER_A6_H_

#include "stm32f10x.h"

void a6_init(u8 idx, u16* irq, u8 len, u8 a6_target);
s8 check(u32 delay);
s8 dial(u8* target, u32 port);
s8 send_data(u8* data, u32 len);
s8 close(void);

void send_cmd(u8* cmd);
#endif
