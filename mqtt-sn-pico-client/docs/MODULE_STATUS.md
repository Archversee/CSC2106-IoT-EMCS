# MicroSD Driver Module Breakdown - Implementation Status

## Completed Modules

### ✅ 1. microsd_internal.h
**Status:** Complete
**Location:** `drivers/microsd_internal.h`
**Contents:**
- Shared global variables declarations
- Error codes
- Logging macro
- Internal function declarations for cross-module use
- Sector cache structure

### ✅ 2. microsd_util.c
**Status:** Complete  
**Location:** `drivers/microsd_util.c`
**Functions:**
- `microsd_add_to_log_buffer()` - Add log messages to buffer
- `microsd_flush_log_to_file()` - Write log buffer to file
- `microsd_list_directory()` - List all files in root directory
- `microsd_hex_dump()` - Hex dump of SD sectors for debugging
- `get_current_time()` - Get timestamp for file entries

**Lines:** ~400 lines

### ✅ 3. microsd_exfat.c
**Status:** Complete
**Location:** `drivers/microsd_exfat.c`
**Functions:**
- `microsd_init_filesystem()` - Initialize exFAT filesystem info
- `microsd_cluster_to_sector()` - Convert cluster number to sector
- `microsd_get_fat_entry_location()` - Calculate FAT entry location
- `microsd_get_next_cluster()` - Get next cluster in chain
- `microsd_find_free_cluster()` - Find free cluster in FAT
- `microsd_mark_cluster_end()` - Mark cluster as end-of-chain
- `microsd_update_allocation_bitmap()` - Update allocation bitmap
- `microsd_allocate_cluster_chain()` - Allocate cluster chain for files
- `expand_directory()` - Expand directory when full (helper)
- `is_cluster_free_in_bitmap()` - Check if cluster is free (helper)
- `find_allocation_bitmap_cluster()` - Find bitmap cluster (helper)

**Lines:** ~650 lines

### ✅ 4. microsd_spi.c  
**Status:** Already exists (partial implementation)
**Location:** `drivers/microsd_spi.c`
**Note:** This file was already present in the codebase. It handles low-level SPI communication.

## Modules to Complete

### ✅ 5. microsd_file.c
**Status:** COMPLETE
**Location:** `drivers/microsd_file.c`
**Lines:** ~850 lines

**Functions to Extract from microsd_driver.c (lines ~1500-2700):**
```c
// File creation and reading
bool microsd_create_file(filesystem_info_t const* const p_fs_info, 
                        char const* const filename,
                        uint8_t const* const p_data, 
                        uint32_t const data_length);

bool microsd_read_file(filesystem_info_t const* const p_fs_info, 
                      char const* const filename,
                      uint8_t* const p_buffer, 
                      uint32_t const buffer_size,
                      uint32_t* const p_bytes_read);

bool microsd_create_large_file_chunked(filesystem_info_t const* const p_fs_info,
                                       char const* const filename,
                                       uint8_t const* const p_chunk_data,
                                       uint32_t const chunk_size,
                                       uint32_t const total_size,
                                       uint32_t const num_chunks);

bool microsd_read_large_file_chunked(filesystem_info_t const* const p_fs_info,
                                     char const* const filename,
                                     uint8_t* const p_chunk_data,
                                     uint32_t const chunk_size,
                                     uint32_t const max_size,
                                     uint32_t* const p_total_read);

// Helper functions
uint16_t microsd_calculate_name_hash(char const* const filename, uint32_t length);
uint16_t microsd_calculate_entry_checksum(uint8_t const* const p_entries, uint32_t count);
bool microsd_write_cluster_chain_data(filesystem_info_t const* const p_fs_info,
                                      uint32_t first_cluster,
                                      uint8_t const* const p_data,
                                      uint32_t data_length);
```

**Key Implementation Notes:**
- Handles directory entry creation (file, stream, name entries)
- Manages filename versioning (basename_N.ext for duplicates)
- Calculates exFAT checksums and name hashes
- Writes data across multiple clusters
- Searches directory for files
- Handles reading files from cluster chains

### ✅ 6. microsd_chunk.c
**Status:** COMPLETE
**Location:** `drivers/microsd_chunk.c`
**Lines:** ~830 lines

**Functions to Extract from microsd_driver.c (lines ~2700-3700):**
```c
// Sector cache (for optimized chunk writes)
sector_cache_t g_sector_cache = {0};

bool microsd_flush_sector_cache(filesystem_info_t const* const p_fs_info);
bool microsd_get_sector_for_write(filesystem_info_t const* const p_fs_info,
                                  uint32_t sector_number,
                                  uint8_t** sector_buffer);
void microsd_mark_sector_dirty(void);
void microsd_invalidate_sector_cache(void);

// Chunk operations
bool microsd_init_chunk_write(filesystem_info_t const* const p_fs_info,
                              char const* const filename,
                              uint32_t const total_chunks,
                              uint32_t const chunk_size,
                              uint32_t const actual_file_size,
                              chunk_metadata_t* const p_metadata);

bool microsd_write_chunk(filesystem_info_t const* const p_fs_info,
                         chunk_metadata_t* const p_metadata,
                         uint32_t const chunk_index,
                         uint8_t const* const p_chunk_data,
                         uint32_t const chunk_data_size);

bool microsd_finalize_chunk_write(filesystem_info_t const* const p_fs_info,
                                  chunk_metadata_t const* const p_metadata);

bool microsd_read_chunk(filesystem_info_t const* const p_fs_info,
                        char const* const filename,
                        uint8_t* const p_chunk_data,
                        uint32_t const chunk_size,
                        uint32_t const chunk_index,
                        uint32_t* const p_bytes_read);

bool microsd_check_all_chunks_received(chunk_metadata_t const* const p_metadata);
```

**Key Implementation Notes:**
- Sector-level caching to reduce read-modify-write operations
- Out-of-order chunk reception with bitmap tracking
- Chunk 0 is metadata (not written to file data)
- Volume dirty bit management (set before writes, clear after)
- Directory entry creation after all chunks received

## Global Variable Management

The modules share the following globals (defined in microsd_spi.c, declared in microsd_internal.h):

```c
microsd_driver_info_t g_driver_info = {0};
microsd_log_level_t g_log_level = MICROSD_LOG_INFO;
char g_log_buffer[LOG_BUFFER_SIZE];
uint32_t g_log_buffer_pos = 0;
bool g_log_to_file_enabled = false;
sector_cache_t g_sector_cache = {0};  // Defined in microsd_chunk.c
```

## CMakeLists.txt Update Needed

Once all modules are created, update the `drivers/CMakeLists.txt` to include:

```cmake
# MicroSD Driver Sources
set(MICROSD_DRIVER_SOURCES
    drivers/microsd_spi.c
    drivers/microsd_exfat.c
    drivers/microsd_file.c      # TO BE CREATED
    drivers/microsd_chunk.c     # TO BE CREATED
    drivers/microsd_util.c
)

# Add to your target
target_sources(your_target PRIVATE ${MICROSD_DRIVER_SOURCES})
```

## Migration Strategy

### For Immediate Use:
1. Keep using the monolithic `microsd_driver.c` for now
2. The new modules are ready but not yet integrated

### For Full Migration:
1. Complete `microsd_file.c` (~1200 lines to extract)
2. Complete `microsd_chunk.c` (~1000 lines to extract)
3. Update CMakeLists.txt to include new modules
4. Remove or rename old `microsd_driver.c`
5. Test compilation and functionality

### Testing Approach:
1. Compile with new modules
2. Test basic file operations first
3. Test chunked file operations
4. Test with MQTT-SN file transfer
5. Verify exFAT filesystem consistency (check dirty bit clearing)

## Size Comparison

**Before:**
- microsd_driver.c: 3776 lines

**After:**
- microsd_spi.c: ~213 lines (already exists)
- microsd_exfat.c: ~650 lines ✅
- microsd_file.c: ~1200 lines 🔨
- microsd_chunk.c: ~1000 lines 🔨  
- microsd_util.c: ~400 lines ✅
- microsd_internal.h: ~200 lines ✅
- **Total: ~3663 lines across 6 files**

Each module is now manageable (~200-1200 lines per file) vs the original 3776-line monolith.

## Dependencies Between Modules

```
microsd_driver.h (public API)
    ↓
microsd_internal.h (shared internals)
    ↓
┌───────────────┬──────────────┬──────────────┬──────────────┐
│  microsd_spi.c │ microsd_exfat.c│ microsd_file.c│ microsd_chunk.c│
│  (low-level)  │  (filesystem)  │  (file ops)   │  (chunking)    │
└───────────────┴──────────────┴──────────────┴──────────────┘
                        ↓
                  microsd_util.c
                   (utilities)
```

## Next Steps

To complete the refactoring:

1. Extract remaining functions from microsd_driver.c:2700-3776 → microsd_chunk.c
2. Extract remaining functions from microsd_driver.c:1500-2700 → microsd_file.c
3. Update CMakeLists.txt
4. Compile and test
5. Remove old microsd_driver.c once verified

**Estimated Time to Complete:** 2-3 hours of focused work to extract and test the remaining ~2200 lines.
