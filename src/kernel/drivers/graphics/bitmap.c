#include <kernel/drivers/graphics/bitmap.h>
#include <kernel/drivers/graphics/framebuffer.h>
#include <kernel/drivers/graphics/fonts/font.h>
#include <types.h>






// putpixels of A-Z bits from right-to-left
static void DrawAlphaBitmap(u16 index, u32 x, u32 y, struct framebuffer_pixel pixel)
{
  u16 temp = 0, pix = 0;
  int one = 1;
  struct framebuffer_pixel black_pixel = {0,0,0};
  for(u8 i = 0; i < BITMAP_SIZE; i++){
    temp = x;
    x += BITMAP_SIZE;
    pix = font[index][i];
    while(one == 1){
      if (pix & 1){
        PutPixel(x, y, pixel);
      }
      else{
        PutPixel(x,y,black_pixel);
      }
      
      
      pix >>= 1;
      x--;  
      if (temp == x){
        break;
      }
    }
    x = temp;
    y++;
  }
}


extern void
bitmapPC(u32 x, u32 y, char ch, struct framebuffer_pixel pixel) // means: bitmap print character
{
  DrawAlphaBitmap((int)ch,  x, y, pixel);
}

extern void 
bitmapPS(u32 x, u32 y, char *str, struct framebuffer_pixel pixel) // means: bitmap print string
{
  u32 index = 0;

  while(str[index]){
    bitmapPC(x, y, str[index], pixel);
    x += BITMAP_SIZE + 1;
    index++;
  }
}
