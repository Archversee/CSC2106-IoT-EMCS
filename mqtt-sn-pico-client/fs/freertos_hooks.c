/*!
 * @file    freertos_hooks.c
 * @brief   FreeRTOS hook functions for filesystem tests
 * @author  INF2004 Project Team
 * @date    2025
 *
 * Minimal FreeRTOS hook implementations required for linking.
 * These are called by the FreeRTOS kernel when errors occur.
 */

#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

/*!
 * @brief FreeRTOS stack overflow hook
 * Called when a task stack overflow is detected
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    (void)xTask;
    printf("ERROR: Stack overflow in task: %s\n", pcTaskName);
    while (1) {
        // Halt on stack overflow
    }
}

/*!
 * @brief FreeRTOS malloc failed hook
 * Called when pvPortMalloc fails to allocate memory
 */
void vApplicationMallocFailedHook(void) {
    printf("ERROR: Memory allocation failed\n");
    while (1) {
        // Halt on malloc failure
    }
}
