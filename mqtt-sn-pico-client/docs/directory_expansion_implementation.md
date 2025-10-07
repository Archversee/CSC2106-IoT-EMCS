# Directory Expansion and Large File Support Implementation

## Overview

I've implemented comprehensive directory expansion and large file support for the microSD driver on the Raspberry Pi Pico W. This enhancement allows the driver to:

1. **Handle large files (2MB+)** across multiple clusters
2. **Automatically expand directories** when they become full
3. **Efficiently utilize the full 16GB storage capacity**

## Key Features Implemented

### 1. Directory Expansion (`expand_directory`)

**Problem Solved:** The original implementation treated the root directory as fixed-size (16 entries), causing "directory full" errors even when plenty of storage space was available.

**Solution:** 
- Automatically allocates new clusters for directories when they become full
- Links new clusters to the existing directory chain via FAT entries
- Updates allocation bitmaps to mark new clusters as used
- Supports unlimited directory growth (within available storage)

**Functions Added:**
- `expand_directory()` - Allocates and links a new cluster to directory chain
- `ensure_directory_space()` - Checks space and expands if needed

### 2. Large File Support

**Problem Solved:** Original implementation only supported files up to one cluster size (~32KB with typical cluster sizes).

**Solution:**
- Calculates required number of clusters based on file size
- Allocates and links multiple clusters in a chain
- Writes data across cluster boundaries seamlessly
- Supports files limited only by available storage space

**Functions Added:**
- `allocate_cluster_chain()` - Allocates multiple linked clusters
- `write_cluster_chain_data()` - Writes data across cluster chain

### 3. Enhanced File Creation Logic

**Improvements:**
- Replaced single-cluster allocation with cluster chain allocation
- Enhanced directory space checking with automatic expansion
- Better error handling and rollback capabilities
- Improved logging for debugging large operations

## Technical Implementation Details

### Directory Expansion Process

1. **Space Check:** When creating a file, check if directory has enough entries
2. **Cluster Allocation:** If space insufficient, find a free cluster
3. **Chain Linking:** Link new cluster to end of directory cluster chain
4. **Initialization:** Clear new cluster and mark as allocated
5. **FAT Update:** Update File Allocation Table with new chain link
6. **Bitmap Update:** Mark new cluster as used in allocation bitmap

### Large File Allocation Process

1. **Size Calculation:** Determine clusters needed based on file size and cluster size
2. **Sequential Allocation:** Find and allocate multiple free clusters
3. **Chain Creation:** Link clusters together in FAT (cluster1 → cluster2 → ... → EOF)
4. **Data Writing:** Write data across cluster boundaries sector by sector
5. **Bitmap Updates:** Mark all allocated clusters as used

### Error Handling and Recovery

- **Rollback Support:** If allocation fails partway, can clean up allocated clusters
- **Atomic Operations:** Directory updates are performed atomically where possible
- **Extensive Logging:** Debug information for troubleshooting large operations
- **Memory Management:** Proper cleanup of large buffers and allocated resources

## Files Modified/Created

### Modified Files:
- `microsd_driver.c`: Core implementation of directory expansion and large file support
- `CMakeLists.txt`: Added large file test executable

### New Files:
- `large_file_test.c`: Comprehensive test for 2MB file creation and directory expansion

## Testing

The implementation includes comprehensive tests:

1. **Large File Test:** Creates a 2MB file with pattern data
2. **Directory Expansion Test:** Creates 20+ files to trigger directory expansion
3. **Integration Test:** Verifies both features work together

## Benefits for 16GB Storage

With these enhancements, the microSD driver can now:

- **Utilize Full Capacity:** Support files and directories using the entire 16GB space
- **Handle IoT Data Logs:** Store large telemetry data files without size restrictions
- **Support Rich Media:** Store images, audio, or other large binary files
- **Scale Directory Structure:** Support hundreds or thousands of files in directories

## Performance Considerations

- **Memory Usage:** Large file operations require sufficient RAM for buffers
- **Write Speed:** Large files may take time to write; progress logging helps track status
- **Cluster Efficiency:** Larger cluster sizes reduce FAT overhead for big files
- **Directory Caching:** Future enhancement could cache directory clusters for better performance

## Usage Examples

```c
/* Create a 2MB file */
uint8_t *large_data = malloc(2 * 1024 * 1024);
/* fill data... */
microsd_create_file(&fs_info, "large_file.bin", large_data, 2 * 1024 * 1024);

/* Create many files - directory expands automatically */
for (int i = 0; i < 100; i++) {
    char filename[64];
    snprintf(filename, sizeof(filename), "file_%03d.txt", i);
    microsd_create_file(&fs_info, filename, data, data_size);
}
```

## Future Enhancements

1. **File Reading:** Implement large file reading across cluster chains
2. **Directory Traversal:** Support reading expanded directories
3. **File Deletion:** Properly clean up cluster chains when deleting large files
4. **Defragmentation:** Optimize cluster allocation for better performance
5. **Wear Leveling:** Distribute writes across the SD card for longevity

This implementation transforms the microSD driver from a simple storage interface into a robust filesystem capable of handling real-world IoT data storage requirements on the full 16GB capacity.