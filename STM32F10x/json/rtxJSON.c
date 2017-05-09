#include "rtxJSON.h"
#include "cJSON.h"
#include "RTL.h"

#define MAX_POOL_SIZE 1024
static u32 mempool[MAX_POOL_SIZE + 3];

void InitMemoryMgr()
{
    cJSON_Hooks hooks;
    hooks.malloc_fn = delegateMalloc;
    hooks.free_fn = delegateFree;

    cJSON_InitHooks(&hooks);

    _init_box(mempool, sizeof(mempool), 20);
}

void* delegateMalloc(u32 size)
{
    u32 delegateSize = size + 4;
    u8* ptr = NULL;
    _declare_box(dmp, delegateSize, 1);
}

void delegateFree(void* ptr)
{
    _free_box(mempool, ((u8*)ptr - 4));
}