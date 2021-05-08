#include <kernel/log.h>
#include <x86.h>
#include "frames.h"

// 8M of phys mem
#define FRAME_COUNT 0x800

// the frame bitmap for the physical memory
static struct frame_bitmap frames;
static u8 map_buffer[0x100];

/* linker */
extern u32 end;

/* exports */

u32 physmem_base = (u32) &end;

extern void
physmem_init(void)
{
	tracef("setting up the physmem manager\n", NULL);
	
	// create the frame map
	frames.frames = map_buffer;
	frames.nframes = FRAME_COUNT;
	tracef("> frame bitmap at [%p]\n", frames.frames)
	tracef("> for %d frames\n", frames.nframes);

	// align base
	tracef("> aligning physmem_base\n", NULL);
	if (physmem_base & 0xfff) physmem_base += 0x1000;
	physmem_base &= 0xfffff000;
	tracef("> final physmem_base at [%p]\n", physmem_base);
}

extern u32
physmem_alloc(void)
{
	u32 frame = frame_first(&frames);
	if (frame == FRAME_COUNT) {
		tracef("out of frames !\n", NULL);
		hang();
	}
	frame_set(&frames, frame);
	u32 paddr = physmem_base + frame * 0x1000;
	tracef("frame [%d] p[%p]\n", frame, paddr);
	return paddr;
}

extern void
physmem_free(u32 paddr)
{
	tracef("freeing p[%p]\n", paddr);
	u32 frame = (paddr - physmem_base) / 0x1000;
	tracef("> frame [%d]\n", frame);
	if (!frame_test(&frames, frame)) {
		tracef("> double free !\n", NULL);
		return;
	}
	frame_clear(&frames, frame);
	tracef("> done\n", NULL);
}