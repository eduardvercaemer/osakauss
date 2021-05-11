#ifndef __STIVALE__STIVALE2_H__
#define __STIVALE__STIVALE2_H__

#include <types.h>

struct stivale2_tag {
    u64 identifier;
    u64 next;
} __attribute__((__packed__));

/* --- Header --------------------------------------------------------------- */
/*  Information passed from the kernel to the bootloader                      */

struct stivale2_header {
    u64 entry_point;
    u64 stack;
    u64 flags;
    u64 tags;
} __attribute__((__packed__));

#define STIVALE2_HEADER_TAG_FRAMEBUFFER_ID 0x3ecc1bc43d0f7971

struct stivale2_header_tag_framebuffer {
    struct stivale2_tag tag;
    u16 framebuffer_width;
    u16 framebuffer_height;
    u16 framebuffer_bpp;
} __attribute__((__packed__));

#define STIVALE2_HEADER_TAG_FB_MTRR_ID 0x4c7bb07731282e00

#define STIVALE2_HEADER_TAG_TERMINAL_ID 0xa85d499b1823be72

struct stivale2_header_tag_terminal {
    struct stivale2_tag tag;
    u64 flags;
} __attribute__((__packed__));

#define STIVALE2_HEADER_TAG_SMP_ID 0x1ab015085f3273df

struct stivale2_header_tag_smp {
    struct stivale2_tag tag;
    u64 flags;
} __attribute__((__packed__));

#define STIVALE2_HEADER_TAG_5LV_PAGING_ID 0x932f477032007e8f

#define STIVALE2_HEADER_TAG_UNMAP_NULL_ID 0x92919432b16fe7e7

/* --- Struct --------------------------------------------------------------- */
/*  Information passed from the bootloader to the kernel                      */

struct stivale2_struct {
#define STIVALE2_BOOTLOADER_BRAND_SIZE 64
    char bootloader_brand[STIVALE2_BOOTLOADER_BRAND_SIZE];

#define STIVALE2_BOOTLOADER_VERSION_SIZE 64
    char bootloader_version[STIVALE2_BOOTLOADER_VERSION_SIZE];

    u64 tags;
} __attribute__((__packed__));

#define STIVALE2_STRUCT_TAG_CMDLINE_ID 0xe5e76a1b4597a781

struct stivale2_struct_tag_cmdline {
    struct stivale2_tag tag;
    u64 cmdline;
} __attribute__((__packed__));

#define STIVALE2_STRUCT_TAG_MEMMAP_ID 0x2187f79e8612de07

#define STIVALE2_MMAP_USABLE                 1
#define STIVALE2_MMAP_RESERVED               2
#define STIVALE2_MMAP_ACPI_RECLAIMABLE       3
#define STIVALE2_MMAP_ACPI_NVS               4
#define STIVALE2_MMAP_BAD_MEMORY             5
#define STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE 0x1000
#define STIVALE2_MMAP_KERNEL_AND_MODULES     0x1001
#define STIVALE2_MMAP_FRAMEBUFFER            0x1002

struct stivale2_mmap_entry {
    u64 base;
    u64 length;
    u32 type;
    u32 unused;
} __attribute__((__packed__));

struct stivale2_struct_tag_memmap {
    struct stivale2_tag tag;
    u64 entries;
    struct stivale2_mmap_entry memmap[];
} __attribute__((__packed__));

#define STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID 0x506461d2950408fa

#define STIVALE2_FBUF_MMODEL_RGB 1

struct stivale2_struct_tag_framebuffer {
    struct stivale2_tag tag;
    u64 framebuffer_addr;
    u16 framebuffer_width;
    u16 framebuffer_height;
    u16 framebuffer_pitch;
    u16 framebuffer_bpp;
    u8  memory_model;
    u8  red_mask_size;
    u8  red_mask_shift;
    u8  green_mask_size;
    u8  green_mask_shift;
    u8  blue_mask_size;
    u8  blue_mask_shift;
} __attribute__((__packed__));

#define STIVALE2_STRUCT_TAG_EDID_ID 0x968609d7af96b845

struct stivale2_struct_tag_edid {
    struct stivale2_tag tag;
    u64 edid_size;
    u8  edid_information[];
} __attribute__((__packed__));

#define STIVALE2_STRUCT_TAG_FB_MTRR_ID 0x6bc1a78ebe871172

#define STIVALE2_STRUCT_TAG_TERMINAL_ID 0xc2b3f4c3233b0974

struct stivale2_struct_tag_terminal {
    struct stivale2_tag tag;
    u32 flags;
    u16 cols;
    u16 rows;
    u64 term_write;
} __attribute__((__packed__));

#define STIVALE2_STRUCT_TAG_MODULES_ID 0x4b6fe466aade04ce

struct stivale2_module {
    u64 begin;
    u64 end;

#define STIVALE2_MODULE_STRING_SIZE 128
    char string[STIVALE2_MODULE_STRING_SIZE];
} __attribute__((__packed__));

struct stivale2_struct_tag_modules {
    struct stivale2_tag tag;
    u64 module_count;
    struct stivale2_module modules[];
} __attribute__((__packed__));

#define STIVALE2_STRUCT_TAG_RSDP_ID 0x9e1786930a375e78

struct stivale2_struct_tag_rsdp {
    struct stivale2_tag tag;
    u64 rsdp;
} __attribute__((__packed__));

#define STIVALE2_STRUCT_TAG_EPOCH_ID 0x566a7bed888e1407

struct stivale2_struct_tag_epoch {
    struct stivale2_tag tag;
    u64 epoch;
} __attribute__((__packed__));

#define STIVALE2_STRUCT_TAG_FIRMWARE_ID 0x359d837855e3858c

#define STIVALE2_FIRMWARE_BIOS (1 << 0)

struct stivale2_struct_tag_firmware {
    struct stivale2_tag tag;
    u64 flags;
} __attribute__((__packed__));

#define STIVALE2_STRUCT_TAG_EFI_SYSTEM_TABLE_ID 0x4bc5ec15845b558e

struct stivale2_struct_tag_efi_system_table {
    struct stivale2_tag tag;
    u64 system_table;
} __attribute__((__packed__));

#define STIVALE2_STRUCT_TAG_KERNEL_FILE_ID 0xe599d90c2975584a

struct stivale2_struct_tag_kernel_file {
    struct stivale2_tag tag;
    u64 kernel_file;
} __attribute__((__packed__));

#define STIVALE2_STRUCT_TAG_KERNEL_SLIDE_ID 0xee80847d01506c57

struct stivale2_struct_tag_kernel_slide {
    struct stivale2_tag tag;
    u64 kernel_slide;
} __attribute__((packed));

#define STIVALE2_STRUCT_TAG_SMBIOS_ID 0x274bd246c62bf7d1

struct stivale2_struct_tag_smbios {
    struct stivale2_tag tag;
    u64 flags;
    u64 smbios_entry_32;
    u64 smbios_entry_64;
} __attribute__((packed));

#define STIVALE2_STRUCT_TAG_SMP_ID 0x34d1d96339647025

struct stivale2_smp_info {
    u32 processor_id;
    u32 lapic_id;
    u64 target_stack;
    u64 goto_address;
    u64 extra_argument;
} __attribute__((__packed__));

struct stivale2_struct_tag_smp {
    struct stivale2_tag tag;
    u64 flags;
    u32 bsp_lapic_id;
    u32 unused;
    u64 cpu_count;
    struct stivale2_smp_info smp_info[];
} __attribute__((__packed__));

#define STIVALE2_STRUCT_TAG_PXE_SERVER_INFO 0x29d1e96239247032

struct stivale2_struct_tag_pxe_server_info {
    struct stivale2_tag tag;
    u32 server_ip;
} __attribute__((__packed__));

#define STIVALE2_STRUCT_TAG_MMIO32_UART 0xb813f9b8dbc78797

struct stivale2_struct_tag_mmio32_uart {
    struct stivale2_tag tag;
    u64 addr;
} __attribute__((__packed__));

#define STIVALE2_STRUCT_TAG_DTB 0xabb29bd49a2833fa

struct stivale2_struct_tag_dtb {
    struct stivale2_tag tag;
    u64 addr;
    u64 size;
} __attribute__((__packed__));

#define STIVALE2_STRUCT_TAG_VMAP 0xb0ed257db18cb58f

struct stivale2_struct_vmap {
    struct stivale2_tag tag;
    u64 addr;
} __attribute__((__packed__));





#endif