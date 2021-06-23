#ifndef BITMAP_H
#define BITMAP_H

#include <types.h>
#include <kernel/drivers/graphics/framebuffer.h>

#define BITMAP_SIZE 8

extern void bitmapPC(u32 , u32 , char,struct framebuffer_pixel pixel );
extern void bitmapPS(u32 , u32 , char *, struct framebuffer_pixel pixel);

#endif
