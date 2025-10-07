# MicroSD Filesystem Driver

A comprehensive exFAT filesystem driver for Raspberry Pi Pico W, designed for INF2004 CS31 MQTT-SN client project.

## Overview

This driver provides full read/write access to exFAT formatted microSD cards, including automatic file versioning, comprehensive logging, and robust error handling. It's specifically designed for IoT applications requiring reliable data storage and debugging capabilities.

## Driver Information

- **Driver ID**: `MICROSD_DRV_v1.0.0`
- **UUID**: `INF2004-CS31-MICROSD-2024`
- **Version**: `v1.0.0`
- **Filesystem Support**: exFAT
- **Platform**: Raspberry Pi Pico W (RP2040)

## Hardware Configuration

### Pin Connections (MAKER-PI-PICO Board)
```
SD Card SPI Interface:
├── MISO (GP12) - Master In Slave Out
├── MOSI (GP11) - Master Out Slave In  
├── SCK  (GP10) - Serial Clock
└── CS   (GP15) - Chip Select
```

### SPI Configuration
- **Interface**: SPI1
- **Clock Speed**: 400kHz (initialization) → 10MHz (operation)
- **Mode**: SPI Mode 0 (CPOL=0, CPHA=0)

## Features

### Core Functionality
- **exFAT Filesystem Support**: Full compliance with exFAT specification
- **File Operations**: Create, read, write files with proper directory entries
- **Automatic Versioning**: Sequential file numbering (file.txt, file_1.txt, file_2.txt, etc.)
- **Directory Management**: Proper exFAT multi-entry directory structures
- **Error Recovery**: Corrupted entry cleanup and robust error handling

### Logging System
- **Dual Output**: Console and SD card file logging
- **Log Levels**: ERROR, WARN, INFO, DEBUG
- **Timestamping**: Millisecond precision using `to_ms_since_boot()`
- **Log Files**: Automatic creation of `{DRIVER_UUID}_LOG.log` files
- **Buffer Management**: 8KB log buffer with overflow protection

### Data Integrity
- **Volume Dirty Bit**: Automatic clearing for filesystem consistency
- **Allocation Bitmap**: Proper cluster allocation and deallocation
- **Directory Validation**: System entry preservation and corruption detection
- **MBR Protection**: Safe partition handling without corrupting boot sectors

## API Reference

### Initialization

```c
#include "microsd_driver.h"

// Initialize the microSD card
bool microsd_init(void);

// Initialize filesystem information
filesystem_info_t fs_info;
bool microsd_init_filesystem(filesystem_info_t* const p_fs_info);
```

### File Operations

#### Creating Files
```c
// Create a file with automatic versioning
const char* filename = "data.txt";
const char* content = "Hello INF2004 CS31!";
bool success = microsd_create_file(&fs_info, filename, 
                                   (uint8_t*)content, strlen(content));
```

#### Reading Files
```c
// Read file contents
uint8_t buffer[256];
uint32_t bytes_read;
bool success = microsd_read_file(&fs_info, "data.txt", 
                                 buffer, sizeof(buffer), &bytes_read);

if (success) {
    printf("Read %lu bytes: %.*s\n", bytes_read, (int)bytes_read, buffer);
}
```

### Logging Configuration

```c
// Set logging level
microsd_set_log_level(MICROSD_LOG_DEBUG);

// Enable file logging (logs will be written to SD card)
microsd_enable_file_logging(true);

// Use logging macros
MICROSD_LOG(MICROSD_LOG_INFO, "MQTT client started");
MICROSD_LOG(MICROSD_LOG_DEBUG, "Sensor reading: %d", sensor_value);
```

### Driver Information

```c
// Get driver statistics
microsd_driver_info_t info;
if (microsd_get_driver_info(&info)) {
    printf("Driver: %s\n", info.driver_id);
    printf("UUID: %s\n", info.driver_uuid);
    printf("Reads: %lu, Writes: %lu\n", info.total_reads, info.total_writes);
}

// Print driver banner
microsd_print_banner();
```

## File Versioning System

The driver automatically handles file versioning to prevent data loss:

```
First creation:  → HELLO_INF2004.txt
Second creation: → HELLO_INF2004_1.txt  
Third creation:  → HELLO_INF2004_2.txt
...and so on
```

### Versioning Logic
1. Check if base filename exists
2. If exists, increment version number and try `filename_N.ext`
3. Continue until unique filename is found
4. Create file with unique name
5. All previous versions are preserved

## exFAT Directory Structure

The driver creates proper exFAT directory entries:

```
File Entry Set (for "HELLO_INF2004.txt"):
├── File Entry (0x85)           - Basic file metadata
├── Stream Extension (0xC0)     - File size, cluster location
├── Name Entry 1 (0xC1)         - First 15 UTF-16 characters
└── Name Entry 2 (0xC1)         - Remaining characters (if needed)
```

### Directory Layout
```
Root Directory:
├── Volume Label Entry (0x83)    - Filesystem label
├── Allocation Bitmap (0x81)     - Cluster usage tracking  
├── Up-case Table (0x82)         - Unicode case conversion
├── [File Entry Sets...]         - User files
└── End-of-Directory (0x00)      - Directory terminator
```

### Detailed exFAT Structure with Hex Data

#### MBR (Master Boot Record) - Block 0
```
Offset  Description                 Hex Data
0x000   Boot Code                   00 00 00 00 ... (446 bytes)
0x1BE   Partition Table Entry 1:
        Status                      00          (inactive)
        Start CHS                   00 00 00    (legacy)
        Type                        07          (exFAT)
        End CHS                     00 00 00    (legacy)
        Start LBA                   00 20 00 00 (8192 sectors)
        Size in sectors             00 00 DB 01 (31168512 sectors)
0x1CE   Partition Entries 2-4       00 00 00 00 ... (48 bytes)
0x1FE   Boot Signature              55 AA
```

#### exFAT Boot Sector - Block 8192 (partition start)
```
Offset  Description                 Hex Data
0x000   Jump Boot                   EB 76 90
0x003   OEM Name                    45 58 46 41 54 20 20 20 ("EXFAT   ")
0x00B   Must be Zero                00 00 00 00 00 00 00 00 (53 bytes)
0x040   Partition Offset            00 20 00 00 00 00 00 00 (8192)
0x048   Volume Length               00 00 DB 01 00 00 00 00 (31168512)
0x050   FAT Offset                  00 08 00 00             (2048)
0x054   FAT Length                  00 10 00 00             (4096)
0x058   Cluster Heap Offset         00 18 00 00             (6144)
0x05C   Cluster Count               FF D7 01 00             (120831)
0x060   Root Directory Cluster      05 00 00 00             (5)
0x064   Volume Serial Number        XX XX XX XX
0x068   File System Revision        00 01                   (1.0)
0x06A   Volume Flags                02 00                   (dirty bit set)
0x06C   Bytes Per Sector Shift      09                      (512 = 2^9)
0x06D   Sectors Per Cluster Shift   06                      (64 = 2^6)
0x06E   Number of FATs              01                      (1 FAT)
0x06F   Drive Select                80                      (first hard disk)
0x070   Percent In Use              00
0x071   Reserved                    00 00 00 00 00 00 00
0x078   Boot Code                   XX XX XX XX ... (390 bytes)
0x1FE   Boot Signature              55 AA
```

#### Root Directory Structure - Cluster 5
```
Entry 0 - Volume Label (0x83):
Offset  Description                 Hex Data
0x00    Entry Type                  83          (Volume Label)
0x01    Character Count             00          (no label)
0x02    Volume Label                00 00 ... 00 (22 bytes UTF-16)
0x18    Reserved                    00 00 00 00 00 00 00 00

Entry 1 - Allocation Bitmap (0x81):
Offset  Description                 Hex Data
0x00    Entry Type                  81          (Allocation Bitmap)
0x01    Bitmap Flags                00
0x02    Reserved                    00 00 00 ... (18 bytes)
0x14    First Cluster               02 00 00 00 (cluster 2)
0x18    Data Length                 00 EE 00 00 00 00 00 00 (60864 bytes)

Entry 2 - Up-case Table (0x82):
Offset  Description                 Hex Data
0x00    Entry Type                  82          (Up-case Table)
0x01    Reserved1                   00 00 00
0x04    Table Checksum              XX XX XX XX
0x08    Reserved2                   00 00 00 ... (12 bytes)
0x14    First Cluster               04 00 00 00 (cluster 4)
0x18    Data Length                 CC 16 00 00 00 00 00 00 (5836 bytes)

Entry 3 - File Entry (0x85) for "HELLO_INF2004.txt":
Offset  Description                 Hex Data
0x00    Entry Type                  85          (File)
0x01    Secondary Count             03          (3 secondary entries)
0x02    Set Checksum                XX XX       (calculated checksum)
0x04    File Attributes             00 00       (normal file)
0x06    Reserved1                   00 00
0x08    Create Timestamp            XX XX XX XX (FAT timestamp)
0x0C    Last Modified Timestamp     XX XX XX XX
0x10    Last Accessed Timestamp     XX XX XX XX
0x14    Create 10ms Increment       00
0x15    Last Modified 10ms          00
0x16    Create UTC Offset           00
0x17    Last Modified UTC Offset    00
0x18    Last Accessed UTC Offset    00
0x19    Reserved2                   00 00 00 00 00 00 00

Entry 4 - Stream Extension (0xC0):
Offset  Description                 Hex Data
0x00    Entry Type                  C0          (Stream Extension)
0x01    General Secondary Flags     01          (allocation possible)
0x02    Reserved1                   00
0x03    Name Length                 12          (18 characters)
0x04    Name Hash                   XX XX       (calculated hash)
0x06    Reserved2                   00 00
0x08    Valid Data Length           19 00 00 00 00 00 00 00 (25 bytes)
0x10    Reserved3                   00 00 00 00
0x14    First Cluster               13 00 00 00 (cluster 19)
0x18    Data Length                 19 00 00 00 00 00 00 00 (25 bytes)

Entry 5 - Name Entry 1 (0xC1):
Offset  Description                 Hex Data
0x00    Entry Type                  C1          (File Name)
0x01    General Secondary Flags     00
0x02    File Name (UTF-16)          48 00 45 00 4C 00 4C 00 4F 00 5F 00 49 00 4E 00
                                    46 00 32 00 30 00 30 00 34 00 2E 00 74 00
                                    ("HELLO_INF2004.t")

Entry 6 - Name Entry 2 (0xC1):
Offset  Description                 Hex Data
0x00    Entry Type                  C1          (File Name)
0x01    General Secondary Flags     00
0x02    File Name (UTF-16)          78 00 74 00 00 00 00 00 00 00 00 00 00 00 00 00
                                    00 00 00 00 00 00 00 00 00 00 00 00 00 00
                                    ("xt" + padding)

Entry 7 - End of Directory:
Offset  Description                 Hex Data
0x00    Entry Type                  00          (End of Directory)
0x01-1F Reserved                    00 00 00 ... (31 bytes)
```

#### File Data - Cluster 19
```
Content: "HELLO INF2004 CS31 GROUP\n"
Hex Data: 48 45 4C 4C 4F 20 49 4E 46 32 30 30 34 20 43 53 33 31 20 47 52 4F 55 50 0A
Padding:  00 00 00 00 00 00 00 ... (fills rest of cluster)
```

#### Allocation Bitmap - Cluster 2
```
Bitmap showing cluster usage (1 = used, 0 = free):
Byte 0:   1F          (clusters 0-4 used: system clusters)
Byte 1:   00          (clusters 5-12 free)
Byte 2:   08          (cluster 19 used: our file)
...       00 00 00 ... (remaining clusters free)
```

## Usage Examples

### Basic File Operations
```c
int main(void) {
    stdio_init_all();
    
    // Initialize driver
    if (!microsd_init()) {
        printf("Failed to initialize microSD\n");
        return -1;
    }
    
    // Initialize filesystem
    filesystem_info_t fs_info;
    if (!microsd_init_filesystem(&fs_info)) {
        printf("Failed to initialize filesystem\n");
        return -1;
    }
    
    // Enable logging to SD card
    microsd_enable_file_logging(true);
    
    // Create a data file
    const char* sensor_data = "Temperature: 25.3°C\nHumidity: 60%\n";
    microsd_create_file(&fs_info, "sensor_log.txt", 
                       (uint8_t*)sensor_data, strlen(sensor_data));
    
    // Read it back to verify
    uint8_t read_buffer[128];
    uint32_t bytes_read;
    if (microsd_read_file(&fs_info, "sensor_log.txt", 
                         read_buffer, sizeof(read_buffer), &bytes_read)) {
        printf("File verification successful!\n");
    }
    
    return 0;
}
```

### MQTT Client Integration
```c
// In your MQTT client code
void mqtt_log_message(const char* topic, const char* payload) {
    static filesystem_info_t fs_info;
    static bool fs_initialized = false;
    
    // Initialize once
    if (!fs_initialized) {
        microsd_init();
        microsd_init_filesystem(&fs_info);
        microsd_enable_file_logging(true);
        fs_initialized = true;
    }
    
    // Create log entry
    char log_entry[256];
    snprintf(log_entry, sizeof(log_entry), 
             "[%lu] Topic: %s, Payload: %s\n", 
             to_ms_since_boot(), topic, payload);
    
    // Write to versioned log file
    microsd_create_file(&fs_info, "mqtt_log.txt", 
                       (uint8_t*)log_entry, strlen(log_entry));
}
```

## Error Handling

The driver provides comprehensive error reporting:

```c
// Check for initialization errors
if (!microsd_init()) {
    // Check card insertion, SPI connections
}

// Check for filesystem errors  
if (!microsd_init_filesystem(&fs_info)) {
    // SD card might need exFAT formatting
}

// Check for file operation errors
if (!microsd_create_file(&fs_info, filename, data, length)) {
    // Directory might be full, check available space
}
```

### Common Issues and Solutions

| Issue | Cause | Solution |
|-------|-------|----------|
| Initialization fails | Card not inserted or bad connections | Check physical connections |
| Not exFAT filesystem | Wrong format | Format SD card as exFAT |
| Directory full | Too many files | Delete old files or use larger SD card |
| File not found | Incorrect filename | Check exact filename including case |
| Write fails | Card write-protected | Remove write protection |

## Performance Characteristics

### Timing Specifications
- **Initialization**: ~500ms (includes SPI setup and card detection)
- **File Creation**: ~50-100ms (depending on filename length)
- **File Reading**: ~20-50ms (for typical file sizes <1KB)
- **Directory Cleanup**: ~10-30ms (when removing corrupted entries)

### Memory Usage
- **RAM**: ~1KB static buffers + 512B per operation
- **Log Buffer**: 8KB for logging system
- **Stack**: ~2KB maximum during operations

## File Formats and Compatibility

### Supported Operations
- File creation with UTF-8 filenames
- File reading (full file or partial)
- Automatic file versioning
- Directory corruption recovery
- exFAT compatibility (Windows/Mac/Linux)

### Limitations
- Single-cluster files only (32KB max on typical cards)
- Root directory operations only (no subdirectories)
- ASCII filenames recommended (UTF-16 conversion is basic)
- No file deletion functionality (preservation by design)

## Testing and Validation

### Demo Program
The `microsd_demo.c` provides comprehensive testing:

```bash
# Compile and run demo
make -j 8
# Flash to Pico and observe serial output
```

### Test Sequence
1. **Initialization Test**: Card detection and SPI communication
2. **Filesystem Test**: exFAT detection and structure validation  
3. **File Creation Test**: Create versioned files
4. **File Reading Test**: Read back and verify content
5. **Logging Test**: Verify log file creation

### Expected Output
```
=== Test Run #N ===
Initializing microSD card...
MicroSD card initialized successfully
Filesystem initialized successfully
Creating file: HELLO_INF2004.txt
File 'HELLO_INF2004.txt' created successfully
Reading file: HELLO_INF2004.txt  
SUCCESS: Read 25 bytes from file 'HELLO_INF2004.txt'
File contents: HELLO INF2004 CS31 GROUP
```

## Troubleshooting

### Debug Logging
Enable maximum verbosity for troubleshooting:
```c
microsd_set_log_level(MICROSD_LOG_DEBUG);
microsd_enable_file_logging(true);
```

### Common Debug Steps
1. **Check Physical Connections**: Verify all SPI pins are connected
2. **Verify SD Card Format**: Must be exFAT (not FAT32 or NTFS)
3. **Check Serial Output**: Look for detailed error messages
4. **Examine Log Files**: Check `INF2004-CS31-MICROSD-2024_LOG.log` on SD card
5. **Test with Different Cards**: Some cards have compatibility issues

### Diagnostic Commands
```c
// Print detailed driver information
microsd_print_banner();

// Check filesystem structure
microsd_init_filesystem(&fs_info);
// Examine debug output for partition/cluster information
```

## Integration Notes

### MQTT-SN Client Usage
- Use for persistent message logging
- Store configuration data
- Debug output archival
- Sensor data buffering

### Memory Management
- Driver uses static allocation only
- No dynamic memory allocation
- Safe for real-time applications
- Predictable memory footprint

### Thread Safety
- Not thread-safe by design
- Use from single thread only
- Consider mutex if multi-threading needed

## License and Credits

Developed for INF2004 CS31 coursework - Singapore Institute of Technology
Driver designed for educational and research purposes.

---

**Note**: This driver is optimized for the specific requirements of the INF2004 CS31 MQTT-SN client project. For production use, additional features like subdirectory support, file deletion, and multi-cluster files may be needed.