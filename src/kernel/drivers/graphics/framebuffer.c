#include <types.h>

#include <kernel/drivers/graphics/framebuffer.h>



struct Sframebuffer framebuffer;


extern void framebufferInit(u64 addr,struct framebuffer_pixel* buffer, u16 width, u16 height, u16 bpp ,u16 pitch){
    framebuffer.buffer = (char *)buffer;
    framebuffer.width = width;
    framebuffer.height = height;
    framebuffer.bpp = bpp;
    framebuffer.pitch = pitch;
    framebuffer.addr = addr;
    memset(framebuffer.backBuffer, 0, sizeof(u8));
}
extern void PutPixel(u16 x, u16 y, struct framebuffer_pixel pixel){
    //this is the equation that should set the postion of the pixel correctly, but it does not work with framebuffer_addr being there.: framebuffer->framebuffer_addr + y * framebuffer->framebuffer_pitch + x * framebuffer->framebuffer_bpp/8
    // we have to do this because the framebuffer is not always flat and if we don't do this it may come out weird.
    unsigned location = y * framebuffer.pitch + x * (framebuffer.bpp/8);
    framebuffer.buffer[location] = pixel.red;
    framebuffer.buffer[location+1] = pixel.green;
    framebuffer.buffer[location+2] = pixel.blue;
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