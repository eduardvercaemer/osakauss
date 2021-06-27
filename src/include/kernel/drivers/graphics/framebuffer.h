#ifndef __FRAMEBUFFER__HEADER__
#define __FRAMEBUFFER__HEADER__
#include <types.h>
struct framebuffer_pixel
{
    u8 blue;
    u8 green;
    u8 red;
    u8 __unused;
}__attribute__((packed));

struct Sframebuffer{
    u64 addr;
    u8* buffer;
    u16 width;
    u16 height;
    struct framebuffer_pixel color;
    u16 pitch;
    u16 bpp;
    u8* backBuffer;
};

extern void PutPixel(u16, u16, struct framebuffer_pixel);
extern void framebufferInit(u64 addr,struct framebuffer_pixel*, u16,u16,u16,u16);
extern struct framebuffer_pixel GeneratePixelFG(u8 red, u8 green, u8 blue);
extern void SetFramebufferColor(struct framebuffer_pixel pixel);

#endif