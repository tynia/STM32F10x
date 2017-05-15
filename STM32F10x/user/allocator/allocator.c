#include "allocator.h"
#include "cJSON.h"

_declare_box(mb8,   8,   5);
_declare_box(mb16,  16,  20);
_declare_box(mb32,  32,  10);
_declare_box(mb64,  64,  10);
_declare_box(mb128, 128, 10);
_declare_box(mb256, 256, 10);

// static u32 mb8[13];
// static u32 mb16[83];
// static u32 mb32[83];
// static u32 mb64[163];
// static u32 mb128[323];
// static u32 mb256[643];

typedef struct _tag_mem_node {
    u32* mbx;
    void* pdata;
} mem_node;

void InitMemoryMgr()
{
    cJSON_Hooks hooks;

    _init_box(mb8,   sizeof(mb8),   8);
    _init_box(mb16,  sizeof(mb16),  16);
    _init_box(mb32,  sizeof(mb32),  32);
    _init_box(mb64,  sizeof(mb64),  64);
    _init_box(mb128, sizeof(mb128), 128);
    _init_box(mb256, sizeof(mb256), 256);

    hooks.malloc_fn = rtxMalloc;
    hooks.free_fn = rtxFree;
    cJSON_InitHooks(&hooks);
}

void* rtxMalloc(size_t size)
{
    size_t delegateSize = size + sizeof(void*);
    void* ptr = NULL;
    u32*  mbx = NULL;
    mem_node* node = NULL;

    if (delegateSize > 256)
    {
        // size greater than 256 is not supported
        return NULL;
    }

    while (delegateSize < 256)
    {
        if (delegateSize <= 8)
        {
            ptr = _alloc_box(mb8);
            if (NULL != ptr)
            {
                mbx = mb8;
            }
        }
        else if (delegateSize > 8 && delegateSize <= 16)
        {
            ptr = _alloc_box(mb16);
            if (NULL != ptr)
            {
                mbx = mb16;
            }
        }
        else if (delegateSize > 16 && delegateSize <= 32)
        {
            ptr = _alloc_box(mb32);
            if (NULL != ptr)
            {
                mbx = mb32;
            }
        }
        else if (delegateSize > 32 && delegateSize <= 64)
        {
            ptr = _alloc_box(mb64);
            if (NULL != ptr)
            {
                mbx = mb64;
            }
        }
        else if (delegateSize > 64 && delegateSize <= 128)
        {
            ptr = _alloc_box(mb128);
            if (NULL != ptr)
            {
                mbx = mb128;
            }
        }
        else if (delegateSize > 128 && delegateSize <= 256)
        {
            ptr = _alloc_box(mb256);
            if (NULL != ptr)
            {
                mbx = mb256;
            }
        }

        if (NULL != ptr)
        {
            node = (mem_node*)ptr;
            node->mbx = mbx;
            node->pdata = (u8*)ptr + sizeof(u32*);
            return node->pdata;
        }
        else
        {
            delegateSize *= 2;
        }
    }

    return NULL;
}

void rtxFree(void* ptr)
{
    u8* addr = (u8*)((u8*)ptr - sizeof(u32*));
    mem_node* node = (mem_node*)addr;
    if (NULL != node->mbx)
    {
        _free_box(node->mbx, addr);
    }
}
