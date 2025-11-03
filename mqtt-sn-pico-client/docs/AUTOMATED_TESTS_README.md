# Automated Unit Tests

This directory contains automated unit test programs that execute the test cases defined in the Core Traceability Matrix with specified execution counts.

## Test Programs

### 1. automated_microsd_test.c

**Purpose**: Automated testing of MicroSD driver functions with error recovery

**Test Coverage**:
- IT-2: MicroSD Initialization (20 executions)
- UT-5: Block I/O operations (50 executions)
- UT-5: Byte I/O with Read-Modify-Write (30 executions)
- IT-2: File operations (25 executions)

**Total**: 125 test executions

**Features**:
- **Timeout Recovery**: Implements 10-second timeout for initialization
- **Automatic Retry**: Up to 3 retry attempts with 2-second recovery delay
- **Error Reporting**: Only reports errors when they occur
- **Progress Tracking**: Shows test progress and final summary
- **Data Verification**: Verifies all read/write operations with checksums

### 2. automated_chunk_test.c

**Purpose**: Automated testing of data frame serialization and chunking system

**Test Coverage**:
- UT-1: Serialize Payload (100 executions)
- UT-2: Deserialize Payload (350 executions)
- UT-3: CRC16 Verification (40 executions)
- UT-4: Serialize Metadata (100 executions)
- UT-5: Deserialize Metadata (80 executions)
- IT-2: Sequence Number Validation (45 executions)

**Total**: 715 test executions

**Features**:
- **CRC Corruption Detection**: Tests CRC16 by intentionally corrupting data
- **Round-trip Verification**: Tests serialize → deserialize → verify cycles
- **Sequence Validation**: Ensures proper ordering of data chunks
- **Progress Reporting**: Minimal output during high iteration tests

## Building the Tests

The tests are integrated into the main CMake build system:

```bash
cd mqtt-sn-pico-client/build
cmake ..
make automated_microsd_test
make automated_chunk_test
```

This will generate:
- `automated_microsd_test.uf2` - Flash to Pico for MicroSD tests
- `automated_chunk_test.uf2` - Flash to Pico for data frame tests

## Running the Tests

### Prerequisites

**For MicroSD tests**:
- Raspberry Pi Pico W with MicroSD breakout board connected:
  - GP10 → SCK
  - GP11 → MOSI
  - GP12 → MISO
  - GP15 → CS
- MicroSD card formatted as exFAT

**For Data Frame tests**:
- Raspberry Pi Pico W (no additional hardware required)

### Execution

1. **Flash the test program**:
   ```bash
   # Put Pico in BOOTSEL mode (hold BOOTSEL, plug in USB)
   cp build/drivers/automated_microsd_test.uf2 /Volumes/RPI-RP2/
   # or
   cp build/fs/automated_chunk_test.uf2 /Volumes/RPI-RP2/
   ```

2. **Connect to serial console**:
   ```bash
   # macOS
   screen /dev/tty.usbmodem* 115200
   
   # Linux
   screen /dev/ttyACM0 115200
   
   # Or use your IDE's serial monitor
   ```

3. **Observe test execution**:
   - Tests will run automatically on startup
   - Progress is shown for each test iteration
   - Errors are reported immediately when they occur
   - Final summary is printed at the end

## Expected Output

### Successful Test Run

```
================================================================================
              AUTOMATED MICROSD DRIVER UNIT TESTS
================================================================================
Test Suite: MicroSD Driver Verification
Based on: Core Traceability Matrix Requirements
Date: 3 November 2025
================================================================================

=== TEST IT-2: MicroSD Initialization ===
Target executions: 20
[IT-2 Init] Iteration 1/20
  Initialization attempt 1/3...
  ✓ Initialized in 245 ms
✓ PASS: MicroSD and filesystem initialized successfully
...

================================================================================
                    AUTOMATED MICROSD TEST SUMMARY
================================================================================

Total test executions: 125
Passed:  125 (100.0%)
Failed:  0 (0.0%)
Errors:  0

Test Breakdown:
  IT-2 (Initialize):      20 executions
  UT-5 (Block I/O):       50 executions
  UT-5 (Byte I/O RMW):    30 executions
  IT-2 (File operations): 25 executions
  ────────────────────────────────────
  Total:                  125 executions

✓ ALL TESTS PASSED!
```

### Test Failure Example

```
[UT-5 Block I/O] Iteration 23/50
✗ FAIL: Data mismatch at offset 127: wrote 0xA5, read 0x00

ERROR: Block read verification failed
```

### Initialization Timeout Recovery

```
[IT-2 Init] Iteration 5/20
  Initialization attempt 1/3...
ERROR: Initialization timeout
  Waiting 2000 ms before retry...
  Initialization attempt 2/3...
  ✓ Initialized in 312 ms
✓ PASS: MicroSD and filesystem initialized successfully
```

## Test Configuration

You can modify test execution counts by editing the `#define` values at the top of each test file:

```c
// In automated_microsd_test.c
#define TEST_INIT_EXECUTIONS 20
#define TEST_BLOCK_IO_EXECUTIONS 50
#define TEST_BYTE_IO_EXECUTIONS 30
#define TEST_FILE_OPS_EXECUTIONS 25

// In automated_chunk_test.c
#define TEST_SERIALIZE_EXECUTIONS 100
#define TEST_DESERIALIZE_EXECUTIONS 350
#define TEST_CRC_EXECUTIONS 40
// ...
```

## Troubleshooting

### MicroSD Tests

**Problem**: All initialization attempts fail
- **Solution**: Check physical connections (SPI pins)
- **Solution**: Verify SD card is formatted as exFAT
- **Solution**: Try a different SD card

**Problem**: Block I/O failures
- **Solution**: SD card may be worn out - try different sectors
- **Solution**: Check power supply stability (SD cards draw significant current)

**Problem**: Byte I/O (RMW) very slow
- **Expected**: This is normal - RMW cycle involves read + modify + write
- **Performance**: Typically 5-10ms per byte operation

### Data Frame Tests

**Problem**: CRC verification failures
- **Expected**: This test intentionally corrupts data to verify CRC detection
- **Verify**: Check that "CRC failed to detect corruption" errors are NOT shown

**Problem**: Compilation errors
- **Solution**: Ensure Pico SDK is properly configured
- **Solution**: Run `cmake ..` from clean build directory

## Integration with Traceability Matrix

These automated tests directly correspond to the test IDs in `CORE_TRACEABILITY_MATRIX.md`:

| Test ID | Function Under Test | Executions | Test File |
|---------|-------------------|------------|-----------|
| IT-2 | microsd_init() | 20× | automated_microsd_test.c |
| UT-5 | microsd_read/write_block() | 50× | automated_microsd_test.c |
| UT-5 | microsd_read/write_byte() | 30× | automated_microsd_test.c |
| IT-2 | microsd_create/read_file() | 25× | automated_microsd_test.c |
| UT-1 | serialize_payload() | 100× | automated_chunk_test.c |
| UT-2 | deserialize_payload() | 350× | automated_chunk_test.c |
| UT-3 | CRC16 verification | 40× | automated_chunk_test.c |
| UT-4 | serialize_metadata() | 100× | automated_chunk_test.c |
| UT-5 | deserialize_metadata() | 80× | automated_chunk_test.c |
| IT-2 | Sequence validation | 45× | automated_chunk_test.c |

**Total**: 840 automated test executions

## Continuous Integration

These tests can be integrated into a CI/CD pipeline:

1. Build test executables as part of build process
2. Flash to hardware test bench
3. Capture serial output
4. Parse for "ALL TESTS PASSED" or "TESTS FAILED"
5. Fail CI build if tests fail

Example parse script:
```bash
if grep -q "ALL TESTS PASSED" serial_output.log; then
    echo "✓ Tests passed"
    exit 0
else
    echo "✗ Tests failed"
    exit 1
fi
```

## Future Enhancements

Potential additions to the test suite:

- **Stress Testing**: Run tests in loops for extended periods
- **Performance Metrics**: Time each operation and log statistics
- **Error Injection**: Deliberately inject errors to test recovery
- **Power Cycle Testing**: Test behavior across power cycles
- **Concurrent Operations**: Test multi-threaded scenarios
- **JSON Report Output**: Generate machine-readable test reports
