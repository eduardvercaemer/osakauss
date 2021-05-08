#include <types.h>
#include <kernel/heap.h>
#include <kernel/log.h>
#include <kernel/paging.h>
#include <kernel/phys.h>

/* heap settings */
#define HEAP_BASE      0x80000000
#define HEAP_INIT_SIZE 0x10000
#define HEAP_MAX_SIZE  0xF0000
#define HEAP_MIN_SIZE  0x10000
#define MIN_ALLOC      0x4

#define BIN_COUNT 9

/* heap structs */

/*
 * A chunk of heap memory.
 */
struct node {
	bool hole;
	u32 size;

	// bin doubly-linked list
	struct node *prev;
	struct node *next;
};

/*
 * End struct of a heap memory chunk.
 */
struct footer {
	struct node *header;
};

/*
 * Heap structure.
 */
struct heap {
	u32 start;
	u32 end;
	struct node *bins[BIN_COUNT];
};

#define OVERHEAD (sizeof (struct node) + sizeof (struct footer))

/* static variables */

struct heap heap;

/* static methods */

static i32 heap_start(struct heap *heap); // done
static i32 heap_expand(struct heap *heap, u32 sz); // done
// TODO : static void heap_contract(struct heap *heap, u32 sz);
static void heap_collapse(struct heap *heap, struct node *node); // done
static void heap_create_footer(struct node *node); // done
// UNUSED static struct node *heap_first_node(struct heap *heap); // done
// UNUSED static struct node *heap_last_node(struct heap *heap); // done
static struct node *heap_prev_node(struct heap *heap, struct node *node); // done
static struct node *heap_next_node(struct heap *heap, struct node *node); // done
static struct footer *heap_get_footer(struct node *node); // done
static void heap_bin_remove(struct heap *heap, struct node *node); // done
static void heap_bin_insert(struct heap *heap, struct node *node); // done
static u32 heap_get_bin_idx(u32 size); // done
static struct node *heap_get_best_fit(struct node *head, u32 sz); // done
static void heap_consume(struct heap *heap, struct node *node, u32 sz); // done

static void * heap_alloc(struct heap *heap, u32 sz); // done
static void heap_free(struct heap *heap, void * ptr); // done

/* static method definitions */

/*
 * Initialize the heap structure and expand to the initial size.
 */
static i32
heap_start(struct heap *heap)
{
	heap->start = HEAP_BASE;
	heap->end   = HEAP_BASE;
	for (u32 i = 0; i < BIN_COUNT; i++) {
		heap->bins[i] = NULL;
	}

	if (heap_expand(heap, HEAP_INIT_SIZE) < 0) {
		return -1;
	}
	tracef("heap started\n", NULL);
	return 0;
}

/*
 * Map new pages to the top of the heap, getting more free memory.
 *
 * Returns
 *   -1 on error, 0 on success
 */
static i32
heap_expand(struct heap *heap, u32 sz)
{
	if (heap->end + sz - heap->start > HEAP_MAX_SIZE) {
		tracef("cannot expand, maxed heap at [%d] bytes\n", heap->end - heap->start);
		return -1;
	}

	u32 end = heap->end;
	u32 top = end;
	u32 dst = top + sz;
	while (dst > top) {
		u32 paddr = physmem_alloc();
		paging_kmap(paddr, top);
		top += 0x1000;
	}
	heap->end = top;

	struct node *new_region;
	new_region = (struct node *) end;
	tracef("> new region at [%p]\n", new_region);
	new_region->hole = true;
	new_region->size = (top - end) - OVERHEAD;
	new_region->prev = NULL;
	new_region->next = NULL;
	heap_create_footer(new_region);

	heap_bin_insert(heap, new_region);
	heap_collapse(heap, new_region);
	return 0;
}

/*
 * Given a heap node, attempt to collapse with its neighbors if possible.
 */
static void
heap_collapse(struct heap *heap, struct node *node)
{
	if (!node) return;

	if (!node->hole) return;

	// collapse with previous
	struct node *prev = heap_prev_node(heap, node);
	tracef("prev node at [%p]\n", prev);
	if (prev) {
		if (prev->hole) {
			// collapse
			heap_bin_remove(heap, prev);
			heap_bin_remove(heap, node);

			prev->size += node->size + OVERHEAD;
			prev->prev = NULL;
			prev->next = NULL;
			heap_create_footer(prev);

			heap_bin_insert(heap, prev);
			node = prev;
		}
	}

	// collapse with next
	struct node *next = heap_next_node(heap, node);
	tracef("next node at [%p]\n", next);
	if (next) {
		if (next->hole) {
			// collapse
			heap_bin_remove(heap, node);
			heap_bin_remove(heap, next);

			node->size += next->size + OVERHEAD;
			node->prev = NULL;
			node->next = NULL;
			heap_create_footer(node);

			heap_bin_insert(heap, node);
		}
	}
}

/*
 *  Given a node strucutre, create its respective footer.
 */
static void
heap_create_footer(struct node *node)
{
	struct footer *foot = heap_get_footer(node);
	foot->header = node;
}

/*
 * Get a pointer to the first node of the heap.
 */
/* UNUSED
static struct node *
heap_first_node(struct heap *heap)
{
	if (heap->start == heap->end) return NULL;
	return (struct heap *) heap->start;
}
*/

/*
 * Get a pointer to the last node of the heap.
 */
/* UNUSED
static struct node *
heap_last_node(struct heap *heap)
{
	if (heap->start == heap->end) return NULL;
	return (struct node *) heap_prev_node(heap, heap->end);
}
*/

/*
 * Get the previous node from a node pointer.
 */
static struct node *
heap_prev_node(struct heap *heap, struct node *node)
{
	struct footer *prev_foot = (struct footer *) ((u32)node - sizeof (struct footer));
	if ((u32)prev_foot < heap->start) return NULL;
	return prev_foot->header;
}

/*
 * Get the next node pointer from a node.
 */
static struct node *
heap_next_node(struct heap *heap, struct node *node)
{
	struct node *next = (struct node *) ((u32)heap_get_footer(node) + sizeof (struct footer));
	if ((u32)next >= heap->end) return NULL;
	return next;
}

/*
 * Given a node structure, get a pointer to its respective footer.
 */
static struct footer *
heap_get_footer(struct node *node)
{
	u32 ptr = (u32) node;
	ptr += node->size;
	ptr += sizeof (struct node);
	//tracef("node [%p] footer [%p]\n", node, ptr);
	return (struct footer *) ptr;
}

/*
 * Remove a node from the bin it is in.
 */
static void
heap_bin_remove(struct heap *heap, struct node *node)
{
	struct node *head, *prev;
	if (!node) return;
	u32 idx = heap_get_bin_idx(node->size);

	head = heap->bins[idx];
	prev = NULL;
	while (head) {
		if (head == node) { // to remove
			if (prev) {
				prev->next = head->next;
			} else {
				heap->bins[idx] = head->next;
			}

			if (head->next) {
				head->next->prev = prev;
			}

			break;
		}

		prev = head;
		head = head->next;
	}
}

/*
 * Given a node structure, insert it to its appropiate bin, keeping it sorted by
 * size.
 */
static void
heap_bin_insert(struct heap *heap, struct node *node)
{
	if (!node) return;
	u32 idx = heap_get_bin_idx(node->size);

	struct node *head = heap->bins[idx];
	if (!head) {
		heap->bins[idx] = node;
		node->prev = NULL;
		node->next = NULL;
		return;
	}

	struct node *prev = NULL;
	while (head) {
		if (node->size < head->size) {
			// insert here
			if (prev) {
				prev->next = node;
				node->prev = prev;
			} else {
				heap->bins[idx] = node;
				node->prev = NULL;
			}

			head->prev = node;
			node->next = head;

			return;
		}

		prev = head;
		head = head->next;
	}

	// insert at end in this case
	node->prev = prev;
	node->next = NULL;
	prev->next = node;
}

/*
 * Get the appropiate bin index for a chunk of given size.
 * TODO: improve here
 */
static u32
heap_get_bin_idx(u32 size)
{
	u32 count = 0;
	u32 size_ = size;
	while (size > 0) {
		size /= 4;
		count++;
	}

	if (count >= BIN_COUNT) count = BIN_COUNT - 1;

	tracef("size [%d] bin [%d]\n", size_, count);
	return count;
}

/*
 * Get the smalles chunk that will fit a size from a list of nodes.
 */
static struct node *
heap_get_best_fit(struct node *head, u32 sz)
{
	while (head) {
		if (head->size >= sz) return head;
		head = head->next;
	}
	return NULL;
}

/*
 * Consume an amount of size from a node, splitting if possible into multiple chunks.
 */
static void
heap_consume(struct heap *heap, struct node *node, u32 sz)
{
	if (!heap) return;
	if (!node) return;

	tracef("consuming [%d] bytes at [%p]\n", sz, node);
	if (!(node->size - sz >= OVERHEAD + MIN_ALLOC)) {
		// no split
		tracef("> no split\n", NULL);
		node->hole = false;
		heap_bin_remove(heap, node);
		return;
	}

	// split
	u32 old_size = node->size;
	heap_bin_remove(heap, node);
	node->hole = false;
	node->size = sz;
	heap_create_footer(node);

	u32 new_size = old_size - sz - OVERHEAD;
	struct node *new = heap_next_node(heap, node);
	tracef("> split into [%d] bytes at [%p]\n", new_size, new);
	new->size = new_size;
	new->hole = true;
	new->prev = NULL;
	new->next = NULL;
	heap_create_footer(new);
	heap_bin_insert(heap, new);
}

/*
 * Create an allocation from the most appropiate bin.
 */
static void *
heap_alloc(struct heap *heap, u32 sz)
{
	tracef("sz [%d]\n", sz);
	u32 idx;
	void * ptr = NULL;
	for (idx = heap_get_bin_idx(sz); idx < BIN_COUNT; idx++) {
		tracef("> bin [%d]\n", idx);
		struct node *fit = heap_get_best_fit(heap->bins[idx], sz);
		if (!fit) continue;

		// we got a fit
		tracef("> fit at [%p]\n", fit);
		ptr = (void *) ((u32)fit + sizeof (struct node));
		heap_consume(heap, fit, sz);
		break;
	}

	if (!ptr) { // warn, loop ?
		tracef("oom, expanding\n", NULL);
		if (heap_expand(heap, sz) < 0) {
			return NULL;
		}
		ptr = heap_alloc(heap, sz);
	}

	return ptr;
}

/*
 * Free a chunk of memory.
 */
static void
heap_free(struct heap *heap, void * ptr)
{
	struct node *node = (struct node *) ((u32)ptr - sizeof (struct node));

	if (node->hole) return; // double free

	node->hole = true;
	node->prev = NULL;
	node->next = NULL;
	heap_bin_insert(heap, node);
	heap_collapse(heap, node);
}

/* exports */

extern i32
heap_init(void)
{
	tracef("Initializing heap\n", NULL);
	tracef("> chunk overhead: %d bytes\n", OVERHEAD);
	tracef("> amount of bins: %d\n", BIN_COUNT);
	tracef("> heap base: [%p]\n", HEAP_BASE);
	tracef("> heap init size: [%x] bytes\n", HEAP_INIT_SIZE);
	tracef("> heap structure at [%p]\n", &heap);

	if (heap_start(&heap) < 0) {
		tracef("> failed to start heap\n", NULL);
		return -1;
	}

	return 0;
}

extern void *
kmalloc(u32 sz)
{
	return kmalloc__(sz, false, NULL);
}

extern void *
kmalloc_a(u32 sz)
{
	return kmalloc__(sz, true, NULL);
}

extern void *
kmalloc_ap(u32 sz, u32* phys)
{
	return kmalloc__(sz, true, phys);
}

/*
 * TODO !!
 * - handle align flag
 * - handle phys argument
 * these are gonna be needed when we will want to eventually
 * allocate new memory tables and directories
 */
extern void *
kmalloc__(u32 sz, bool align, u32 *phys)
{
	/* for now, silent the unused arg warnings */
	align = align;
	phys = phys;

	return heap_alloc(&heap, sz);
}

extern void
kfree(void * ptr)
{
	heap_free(&heap, ptr);
}