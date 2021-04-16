/*
 * virutal memory allocation
 */
#include <kernel/heap.h>
#include <kernel/log.h>
#include <types.h>
#include <x86.h>
#include <stdlib.h>
#include "frames.h"

/* frame allocation */
/*
 * we can use a frame_bitmap to manage the page allocation for virutal memory,
 * requesting the first_frame will yield the first page that is currently free
 */

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

/* exports */

// Set a bit in the frames bitset
extern void
frame_set(struct frame_bitmap *map, u32 frame)
{
	u32 *frames = map->frames;
	
	u32 idx = INDEX_FROM_BIT(frame);
	u32 off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
	
	//tracef("set [%d]\n", frame);
}

// Clear a bit in the frames bitset
extern void
frame_clear(struct frame_bitmap *map, u32 frame)
{
	u32 *frames = map->frames;
	
	u32 idx = INDEX_FROM_BIT(frame);
	u32 off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
	
	//tracef("clear [%d]\n", frame);
}

// Test if a bit is set.
extern bool
frame_test(struct frame_bitmap *map, u32 frame)
{
	u32 *frames = map->frames;
	
	u32 idx = INDEX_FROM_BIT(frame);
	u32 off = OFFSET_FROM_BIT(frame);
	bool ret = frames[idx] & (0x1 << off);
	
	//tracef("test [%d]: %s\n", frame, ret ? "set" : "clear");
	
	return ret;
}

// Find the first free frame.
extern u32
frame_first(struct frame_bitmap *map)
{
	u32 *frames = map->frames;
	u32 nframes = map->nframes;
	
	u32 i, j;
	u32 frame;
	for (i = 0; i < INDEX_FROM_BIT(nframes); i++) {
		if (frames[i] == 0xffffffff) { // nothing free in here
			continue;
		}
		
		for (j = 0; j < 32; j++) {
			u32 toTest = 0x1 << j;
			if ( !(frames[i] & toTest) ) {
				frame = i*4*8+j;
				goto done;
			}
		}
	}
	
	// nothing found !
	return map->nframes;
	
	done:
	//logf("%s: empty frame at %d\n", __func__, frame);
	return frame;
}
