#include <stdio.h>

#include "microsd_driver.h"
#include "pico/stdlib.h"

// Main demo function
int main() {
    stdio_init_all();

    printf("MicroSD Driver Demo\n");
    printf("===================\n");

    // Initialize microSD card
    printf("Initializing microSD card...\n");
    if (!microsd_init()) {
        printf("Failed to initialize microSD card\n");
        return -1;
    }
    printf("MicroSD card initialized successfully\n");

    // Test data and address
    uint8_t write_data = 0xAB;
    uint8_t read_data = 0x00;
    uint32_t test_address = 0x1000;  // Test at address 4096

    printf("\nTesting bidirectional data transfer...\n");
    printf("Test address: 0x%08X\n", test_address);

    // Write byte to microSD
    printf("\nWriting test byte 0x%02X...\n", write_data);
    if (!microsd_write_byte(test_address, write_data)) {
        printf("Write operation failed!\n");
        return -1;
    }
    printf("Successfully wrote byte 0x%02X to microSD\n", write_data);

    // Read byte from microSD
    printf("\nReading test byte...\n");
    if (!microsd_read_byte(test_address, &read_data)) {
        printf("Read operation failed!\n");
        return -1;
    }
    printf("Successfully read byte 0x%02X from microSD\n", read_data);

    // Verify data integrity
    printf("\nData verification:\n");
    printf("Written: 0x%02X\n", write_data);
    printf("Read:    0x%02X\n", read_data);

    if (write_data == read_data) {
        printf("SUCCESS: Data transfer verified!\n");
        printf("Bidirectional communication with microSD card working correctly.\n");
    } else {
        printf("ERROR: Data mismatch detected!\n");
        return -1;
    }

    // Additional test with different data
    printf("\n--- Additional Test ---\n");
    uint8_t test_data[] = {0x12, 0x34, 0x56, 0x78, 0xAB};
    uint32_t base_address = 0x2000;
    bool all_passed = true;

    printf("Testing multiple bytes at different addresses...\n");

    for (int i = 0; i < 5; i++) {
        uint32_t addr = base_address + i;
        uint8_t read_val = 0x00;

        // Write
        if (!microsd_write_byte(addr, test_data[i])) {
            printf("Failed to write byte %d\n", i);
            all_passed = false;
            continue;
        }

        // Read back
        if (!microsd_read_byte(addr, &read_val)) {
            printf("Failed to read byte %d\n", i);
            all_passed = false;
            continue;
        }

        // Verify
        if (test_data[i] != read_val) {
            printf("Mismatch at byte %d: wrote 0x%02X, read 0x%02X\n",
                   i, test_data[i], read_val);
            all_passed = false;
        } else {
            printf("Byte %d: 0x%02X ✓\n", i, test_data[i]);
        }
    }

    if (all_passed) {
        printf("\nALL TESTS PASSED!\n");
        printf("MicroSD driver demonstrates successful bidirectional data transfer.\n");
    } else {
        printf("\nSome tests failed!\n");
        return -1;
    }

    return 0;
}
