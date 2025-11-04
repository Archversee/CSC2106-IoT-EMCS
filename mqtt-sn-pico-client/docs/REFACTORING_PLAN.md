# MicroSD Driver Refactoring Plan

## Overview
The `microsd_driver.c` file (3776 lines) is being split into 5 modular files for better maintainability.

## Module Breakdown

### 1. microsd_spi.c (Already Exists - Partial)
**Lines from original: ~1-500**
**Functions:**
- `microsd_init()` - Initialize SD card via SPI
- `microsd_read_block()` - Read 512-byte block
- `microsd_write_block()` - Write 512-byte block
- `microsd_read_byte()` - Read single byte
- `microsd_write_byte()` - Write single byte
- `microsd_spi_transfer()` - Low-level SPI transfer
- `microsd_send_command()` - Send SD card command
- `microsd_cs_select()` / `microsd_cs_deselect()` - Chip select control
- `microsd_get_driver_info()` - Get driver statistics
- `microsd_set_log_level()` - Set logging level
- `microsd_print_banner()` - Print driver info
- `microsd_enable_file_logging()` - Enable file logging

### 2. microsd_exfat.c (NEW)
**Lines from original: ~500-1500**
**Functions:**
- `microsd_init_filesystem()` - Initialize exFAT filesystem
- `microsd_cluster_to_sector()` - Convert cluster to sector number
- `microsd_find_free_cluster()` - Find free cluster in FAT
- `microsd_mark_cluster_end()` - Mark cluster as end-of-chain
- `microsd_update_allocation_bitmap()` - Update allocation bitmap
- `microsd_get_next_cluster()` - Get next cluster in chain
- `microsd_allocate_cluster_chain()` - Allocate chain for large files
- `microsd_get_fat_entry_location()` - Calculate FAT entry location
- Helper functions for directory expansion, bitmap management

### 3. microsd_file.c (NEW)
**Lines from original: ~1500-2500**
**Functions:**
- `microsd_create_file()` - Create file in exFAT
- `microsd_read_file()` - Read file from exFAT
- `microsd_create_large_file_chunked()` - Create large file
- `microsd_read_large_file_chunked()` - Read large file
- `microsd_calculate_name_hash()` - Calculate exFAT name hash
- `microsd_calculate_entry_checksum()` - Calculate entry checksum
- `microsd_write_cluster_chain_data()` - Write data across clusters
- `get_current_time()` - Get timestamp for file entries

### 4. microsd_chunk.c (NEW)
**Lines from original: ~2500-3500**
**Functions:**
- `microsd_init_chunk_write()` - Initialize chunked writing
- `microsd_write_chunk()` - Write single chunk (out-of-order)
- `microsd_finalize_chunk_write()` - Finalize and create directory entry
- `microsd_read_chunk()` - Read chunk from file
- `microsd_check_all_chunks_received()` - Check completion
- Sector cache functions: flush, get, mark dirty, invalidate

### 5. microsd_util.c (NEW)
**Lines from original: ~3500-3776**
**Functions:**
- `microsd_list_directory()` - List files in root directory
- `microsd_hex_dump()` - Dump sector data in hex
- `microsd_add_to_log_buffer()` - Add log to buffer
- `microsd_flush_log_to_file()` - Flush log buffer to file

### 6. microsd_internal.h (NEW)
**Shared internal definitions:**
- Global variables (driver info, log level, log buffer, sector cache)
- Error codes
- Logging macro
- Internal function declarations for cross-module calls

## Benefits

1. **Maintainability**: Each module is ~500-1000 lines (manageable size)
2. **Separation of Concerns**: Clear responsibility per module
3. **Testability**: Each module can be unit tested independently
4. **Reusability**: Modules can be used independently if needed
5. **Compilation**: Faster incremental compilation

## Migration Notes

- Public API remains unchanged (microsd_driver.h)
- All existing function names and signatures preserved
- CMakeLists.txt needs update to include new source files
- No changes needed to calling code

## Build Instructions

Add to CMakeLists.txt:
```cmake
add_library(microsd_driver
    drivers/microsd_spi.c
    drivers/microsd_exfat.c
    drivers/microsd_file.c
    drivers/microsd_chunk.c
    drivers/microsd_util.c
)
```
