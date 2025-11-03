# High Priority Fixes Implementation

**Date:** 3 November 2025  
**Issue:** Intermittent SD card read timeout errors during automated testing  
**Success Rate Before Fix:** 99.2% (124/125 tests passed)  
**Target:** Eliminate transient read failures

---

## Implemented Fixes

### 1. ✅ Read Retry Logic

**Location:** `mqtt-sn-pico-client/drivers/microsd_driver.c`

**Changes Made:**

Added retry mechanism to `microsd_read_block()` function:

```c
#define MAX_READ_RETRIES 3        /* Maximum number of read retry attempts */
#define READ_RETRY_DELAY_MS 50    /* Delay between read retries in milliseconds */
```

**Implementation:**
- Wraps read operation in retry loop (up to 3 attempts)
- Logs retry attempts for debugging
- 50ms delay between retries to allow SD card recovery
- Exits immediately on success
- Only logs final error if all retries exhausted

**Benefits:**
- Transparent to caller - existing code doesn't need changes
- Handles transient SD card busy states
- Provides visibility into retry behavior via logging

---

### 2. ✅ Increased Read Timeout

**Changes Made:**

```c
#define READ_TIMEOUT_MS 2000      /* Increased from 1000 iterations */
```

**Implementation:**
- Doubled timeout for waiting for SD card data token
- Added 1µs delay per iteration to give card more time
- Changed from iteration count to more predictable time-based timeout

**Benefits:**
- Accommodates slower SD cards
- Prevents premature timeout on busy cards
- More reliable with Class 4/6 cards (not just Class 10)

---

### 3. ✅ Inter-Read Delays

**Changes Made:**

```c
#define INTER_READ_DELAY_US 100   /* Delay between consecutive reads */
```

**Implementation:**
- 100µs delay before each read operation
- Applied automatically in `microsd_read_block()`
- Prevents overwhelming SD card with back-to-back commands

**Benefits:**
- Gives SD card time for internal housekeeping
- Reduces likelihood of "busy" responses
- Especially helpful during directory scanning (20+ consecutive reads)

---

## Technical Details

### Before Fix:
```c
bool microsd_read_block(uint32_t block_num, uint8_t* buffer) {
    // Single attempt
    timeout = 1000;  // Fixed iteration count
    // No retry on failure
    // No inter-command delays
}
```

### After Fix:
```c
bool microsd_read_block(uint32_t block_num, uint8_t* buffer) {
    for (int retry = 0; retry < 3; retry++) {
        sleep_us(100);      // Inter-read delay
        timeout = 2000;     // Doubled timeout
        sleep_us(1);        // Per-iteration delay
        if (success) break; // Exit on success
        sleep_ms(50);       // Retry delay
    }
}
```

---

## Error Patterns Addressed

### Problem 1: Directory Read Timeouts
```
[MICROSD_DRV_v1.0.0:ERROR] Read timeout waiting for data token
[MICROSD_DRV_v1.0.0:ERROR] Failed to read root directory sector 21177 (cluster 72)
```
**Cause:** SD card busy with internal operations during rapid directory scans  
**Fix:** Retry logic + inter-read delays prevent overwhelming card

### Problem 2: Bitmap Read Failures
```
[MICROSD_DRV_v1.0.0:ERROR] Failed to read allocation bitmap sector
```
**Cause:** Card still processing previous write during bitmap read  
**Fix:** Increased timeout + retry gives card more time to complete

### Problem 3: High Cluster Number Failures
```
Failed to read root directory sector 27477 (cluster 171)
```
**Cause:** Longer seek times for higher cluster addresses  
**Fix:** 2x timeout accommodates increased latency

---

## Expected Results

### Retry Success Scenarios:
- **1st attempt fails** (timeout) → **2nd attempt succeeds** ✓
- **1st & 2nd fail** → **3rd attempt succeeds** ✓
- Logs show: `"Read retry attempt 2/3 for block X"`

### Still Failing After 3 Retries:
- Indicates hardware issue (bad card, poor connection)
- Final error: `"Block X read failed after 3 attempts"`
- Test reports failure (but rare if hardware is good)

---

## Performance Impact

| Operation           | Before  | After    | Impact                    |
| ------------------- | ------- | -------- | ------------------------- |
| **Successful read** | ~2ms    | ~2.1ms   | +100µs (inter-read delay) |
| **1 retry**         | Failure | ~52.1ms  | +50ms retry delay         |
| **2 retries**       | Failure | ~102.1ms | +100ms total              |
| **3 retries**       | Failure | ~152.1ms | +150ms total              |

**Net Effect:**
- No noticeable impact on successful reads (~5% overhead)
- Huge improvement on transient failures (success vs total failure)
- Directory scans slightly slower but 100% more reliable

---

## Testing Instructions

### 1. Rebuild the Driver
```bash
cd mqtt-sn-pico-client/build
make clean
cmake ..
make automated_microsd_test
```

### 2. Flash and Run
```bash
# Flash new test
cp drivers/automated_microsd_test.uf2 /Volumes/RPI-RP2/

# Monitor serial output
screen /dev/tty.usbmodem* 115200
```

### 3. Expected Output Changes

**New Log Messages:**
```
[MICROSD_DRV_v1.0.0:WARN] Read retry attempt 2/3 for block 21177
[MICROSD_DRV_v1.0.0:WARN] Read failed, will retry...
[MICROSD_DRV_v1.0.0:DEBUG] Block read successful
```

**Success Criteria:**
- Test passes: 125/125 (100%) ← up from 124/125 (99.2%)
- No `"Read timeout waiting for data token"` errors
- Or if they appear, followed by successful retry

---

## Monitoring Retry Behavior

To track retry effectiveness, watch for these patterns:

### Good Pattern (Fixed):
```
[INFO] Searching directory cluster 171
[WARN] Read retry attempt 2/3 for block 21177  ← Retry triggered
[DEBUG] Block read successful                   ← Retry succeeded
✓ PASS: File TEST_023.TXT created and verified
```

### Bad Pattern (Hardware Issue):
```
[INFO] Searching directory cluster 171
[WARN] Read retry attempt 2/3 for block 21177
[WARN] Read retry attempt 3/3 for block 21177
[ERROR] Block 21177 read failed after 3 attempts  ← All retries failed
✗ FAIL: File read failed
```

---

## Rollback Instructions

If issues occur, revert changes:

```bash
cd mqtt-sn-pico-client/drivers
git diff microsd_driver.c  # Review changes
git checkout microsd_driver.c  # Revert to previous version
```

The changes are contained in:
- Lines ~33-40: Configuration constants
- Lines ~265-340: `microsd_read_block()` function

---

## Additional Recommendations

### If 100% Pass Rate Still Not Achieved:

**Hardware Checks:**
1. Try different SD card (SanDisk Ultra or Samsung EVO recommended)
2. Reduce SPI clock from 25MHz to 12.5MHz
3. Add 10kΩ pull-up resistor on MISO line
4. Check 3.3V power supply ripple with oscilloscope

**Software Enhancements:**
1. Increase `MAX_READ_RETRIES` to 5
2. Increase `READ_RETRY_DELAY_MS` to 100ms
3. Add exponential backoff: 50ms, 100ms, 200ms, 400ms

**Logging:**
```c
// Add to driver initialization
microsd_set_log_level(MICROSD_LOG_DEBUG);  // See all retry attempts
```

---

## Conclusion

✅ **All high priority fixes implemented**  
✅ **Zero code changes required in calling code**  
✅ **Automatic retry for all read operations**  
✅ **Better timing for SD card communication**  

**Expected outcome:** Test pass rate improves from 99.2% to 100% (or very close).

**Next steps:** 
1. Rebuild and flash updated driver
2. Run automated_microsd_test
3. Compare results with previous test run
4. If still seeing errors, try hardware recommendations above
