# MicroSD Driver Tests

## Overview

This directory contains automated tests for validating the FatFS microSD driver implementation on Raspberry Pi Pico W.

## Test Files

### `multi_file_write_test.c`

Comprehensive test suite for multiple file write operations using the basic FatFS drivers.

**Purpose:** Validate FatFS driver stability, reliability, and performance under various file operation scenarios.

**Test Coverage:**
- **MT-1: Sequential File Writes** (10 executions)
  - Tests sequential write operations to multiple files
  - Verifies data integrity after write
  - File size: 1KB each

- **MT-2: Concurrent Open Files** (5 executions)
  - Tests opening multiple file handles simultaneously
  - Validates concurrent write operations
  - Up to 5 files open at once

- **MT-3: Large File Writes** (5 executions)
  - Tests writing large files (64KB each)
  - Validates proper handling of multi-block writes
  - Verifies file size after write completion

- **MT-4: Small File Writes - Stress Test** (20 executions)
  - Rapid creation and deletion of small files
  - Stress tests file allocation table operations
  - File size: 128 bytes each

- **MT-5: File Operations** (8 executions)
  - Tests rename, delete, stat operations
  - Validates file existence checks
  - Tests file metadata retrieval

- **MT-6: Directory Operations** (5 executions)
  - Tests directory creation and nesting
  - Validates directory listing
  - Tests file operations within directories

**Total Test Executions:** 53

## Building

From the project build directory:

```bash
cd mqtt-sn-pico-client/build
cmake ..
make multi_file_write_test
```

This will generate `multi_file_write_test.uf2` in the build output.

## Running

1. Connect your Raspberry Pi Pico W via USB
2. Put Pico into BOOTSEL mode (hold BOOTSEL button while connecting)
3. Copy `multi_file_write_test.uf2` to the Pico mass storage device
4. Open a serial terminal (115200 baud) to view test output
5. Tests will start automatically after 9 seconds

## Expected Output

The test suite provides colored, real-time output showing:
- Test progress indicators
- Pass/fail status for each test
- Detailed error messages on failure
- Final summary with statistics

Example output:
```
================================================================================
          MULTI-FILE WRITE TEST - FatFS Driver Validation
================================================================================

✓ MicroSD initialized successfully

=== TEST MT-1: Sequential File Writes ===
✓ PASS: Sequential write 10/10 verified

=== TEST MT-2: Concurrent Open Files ===
✓ PASS: 5 concurrent files written successfully

...

================================================================================
            MULTI-FILE WRITE TEST SUMMARY
================================================================================

Total test executions: 53
Passed:  53 (100.0%)
Failed:  0 (0.0%)
Errors:  0

ALL TESTS PASSED! ✓
================================================================================
```

## Hardware Requirements

- Raspberry Pi Pico W
- MicroSD card module (using Cytron Pico W Kit pinout)
- MicroSD card (formatted as FAT32)

## Pin Configuration

The test uses the default Cytron Pico W Kit SD card pinout (defined in `hw_config.c`):
- SPI1 interface
- MISO: GPIO 12
- MOSI: GPIO 11
- SCK: GPIO 10
- CS: GPIO 15

## Troubleshooting

**MicroSD initialization fails:**
- Ensure SD card is properly inserted
- Verify SD card is formatted as FAT32
- Check wiring connections
- Try different SD card

**Tests fail sporadically:**
- Check SD card for corruption
- Verify stable power supply
- Ensure SD card has sufficient free space

**Compilation errors:**
- Ensure Pico SDK is properly configured
- Check that all dependencies are built
- Verify CMake configuration is correct

## Integration with CI/CD

This test can be integrated into automated testing pipelines:
1. Build the test executable
2. Flash to Pico W hardware
3. Capture serial output
4. Parse test results from summary section

## Future Enhancements

Potential additions:
- Performance benchmarking (read/write speeds)
- Fragmentation testing
- Power failure simulation
- Multi-threaded file access tests
- Wear leveling validation

## References

- [FatFS Documentation](http://elm-chan.org/fsw/ff/)
- [no-OS-FatFS-SD-SDIO-SPI-RPi-Pico Library](https://github.com/carlk3/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico)
- INF2004 Project Documentation: `/docs/`
