#ifndef __PHYS_HEADER__
#define __PHYS_HEADER__
#include <types.h>


typedef struct{
    u32 amount;
    u32 start;
    u32 end;
    u32 realSize;
}block;


struct frame{
    u8 *frames;
    u32 frameCount;
    block *blocks;
};





extern void physmem_init();
extern void *physmem_alloc(u32);
extern void physmem_free(u32);
#endif