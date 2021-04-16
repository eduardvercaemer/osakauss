#pragma once

struct frame_bitmap {
	u32 *frames;
	u32 nframes;
};

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

// Set a bit in the frames bitset
extern void
frame_set(struct frame_bitmap *map, u32 frame);

// Clear a bit in the frames bitset
extern void
frame_clear(struct frame_bitmap *map, u32 frame);

// Test if a bit is set.
extern bool
frame_test(struct frame_bitmap *map, u32 frame);

// Find the first free frame.
extern u32
frame_first(struct frame_bitmap *map);
