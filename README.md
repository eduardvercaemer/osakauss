# osakauss

our super awesome kernel and user space system

### memory layout

- The kernel is loaded at `0x00100000`.
- `kmalloc` initially uses 'placement' allocation at the end of the kernel
  (this allocations are _not_ `kfree`'ed).
- `paging_init` creates the kernel directory, identity-mapping everything currently
  in use (i.e. from `0x00000000` to wherever the placement allocator is currently).
- At this point, `kmalloc` works as intended, heap allocation, mapped at `0x00800000`.

#### phys. memory bookkeeping

- After initializing paging, the physical memory bookkeper keeps track of the free
  memory after the 'placement' allocator.

### Boot process

- paging init
  - physmem alloc's
    - pre physmem init
    - early allocs
    - non free-able
  - kernel map page
    - more physmem alloc's
  - switch dir
- physmem init
  - reserves more phys memory (identity mapped)
  - sets physmem ready
  - at this point, physmem allocs are free-able
- heap init
  - todo
- syscall init
  - broken