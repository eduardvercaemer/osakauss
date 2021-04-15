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