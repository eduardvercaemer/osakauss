#include <types.h>

#include <kernel/drivers/graphics/framebuffer.h>



struct Sframebuffer framebuffer;


extern void framebufferInit(struct framebuffer_pixel* buffer, u16 width, u16 height, u16 bpp ,u16 pitch){
    framebuffer.buffer = buffer;
    framebuffer.width = width;
    framebuffer.height = height;
    framebuffer.bpp = bpp;
    framebuffer.pitch = pitch;
}
extern void PutPixel(u16 x, u16 y, struct framebuffer_pixel pixel){
    framebuffer.buffer[x+y*framebuffer.width] = pixel;
}
extern struct framebuffer_pixel 
GeneratePixelFG(u8 red, u8 green, u8 blue)// means: generate pixel forground
{
    struct framebuffer_pixel pixel = {blue,green,red};
    return pixel;
}
extern void
SetFramebufferColor(struct framebuffer_pixel pixel){
    framebuffer.color = pixel;
}