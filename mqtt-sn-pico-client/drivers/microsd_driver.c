/*!
 * @file    microsd_driver.c
 * @brief   High-level MicroSD interface wrapper for no-OS-FatFS library
 * @author  INF2004 Project Team
 * @date    2025
 *
 * This module provides a simplified, high-level interface to the FatFS library
 * for microSD card operations on Raspberry Pi Pico W.
 */

#include "microsd_driver.h"

#include <stdio.h>
#include <string.h>

#include "ff.h"
#include "sd_card.h"

/*! Driver state */
static struct {
    bool initialized;
    bool driver_ready;
    uint32_t total_reads;
    uint32_t total_writes;
    uint32_t last_error;
    FATFS fs;
} g_sd_state = {0};

/*! Buffer for read operations (32KB for optimal performance) */
#define READ_BUFFER_SIZE (32768U)
static uint8_t g_read_buffer[READ_BUFFER_SIZE];

/*!
 * @brief Initialize the microSD driver and mount the filesystem
 * @return true on success, false on failure
 */
bool microsd_driver_init(void) {
    if (g_sd_state.initialized) {
        printf("MicroSD driver already initialized\n");
        return true;
    }

    printf("Initializing SD card driver...\n");

    // Initialize SD card hardware driver
    if (!sd_init_driver()) {
        printf("✗ ERROR: SD card hardware initialization failed\n");
        g_sd_state.last_error = 1;
        return false;
    }

    g_sd_state.driver_ready = true;
    printf("✓ SD card hardware initialized\n");

    // Mount the filesystem
    FRESULT fr = f_mount(&g_sd_state.fs, "", 1);
    if (fr != FR_OK) {
        printf("✗ ERROR: Failed to mount filesystem (error %d)\n", fr);
        g_sd_state.last_error = fr;
        return false;
    }

    g_sd_state.initialized = true;
    printf("✓ Filesystem mounted successfully\n");

    return true;
}

/*!
 * @brief Check if microSD card is present and accessible
 * @return true if card is present and mounted, false otherwise
 */
bool microsd_driver_is_present(void) {
    if (!g_sd_state.initialized) {
        return false;
    }

    // Try to get volume information as a presence check
    FATFS *fs;
    DWORD fre_clust;
    FRESULT fr = f_getfree("0:", &fre_clust, &fs);

    return (fr == FR_OK);
}

/*!
 * @brief Unmount the filesystem
 * @return true on success, false on failure
 */
bool microsd_driver_unmount(void) {
    if (!g_sd_state.initialized) {
        return true; // Already unmounted
    }

    FRESULT fr = f_unmount("");
    if (fr != FR_OK) {
        printf("✗ WARNING: Failed to unmount filesystem (error %d)\n", fr);
        g_sd_state.last_error = fr;
        return false;
    }

    g_sd_state.initialized = false;
    printf("✓ Filesystem unmounted\n");
    return true;
}

/*!
 * @brief Get filesystem information
 * @param total_space Output: total space in bytes
 * @param free_space Output: free space in bytes
 * @return true on success, false on failure
 */
bool microsd_driver_get_space(uint64_t *total_space, uint64_t *free_space) {
    if (!g_sd_state.initialized) {
        return false;
    }

    FATFS *fs;
    DWORD fre_clust;
    FRESULT fr = f_getfree("0:", &fre_clust, &fs);

    if (fr != FR_OK) {
        g_sd_state.last_error = fr;
        return false;
    }

    // Calculate total and free space in bytes
    uint64_t total_sectors = (fs->n_fatent - 2) * fs->csize;
    uint64_t free_sectors = fre_clust * fs->csize;

#if FF_MAX_SS != FF_MIN_SS
    uint32_t sector_size = fs->ssize;
#else
    uint32_t sector_size = FF_MAX_SS;
#endif

    if (total_space) {
        *total_space = total_sectors * sector_size;
    }
    if (free_space) {
        *free_space = free_sectors * sector_size;
    }

    return true;
}

/*!
 * @brief Open a file for reading or writing
 * @param file Pointer to FIL structure
 * @param filename Filename to open
 * @param mode Access mode (FA_READ, FA_WRITE, FA_CREATE_ALWAYS, etc.)
 * @return true on success, false on failure
 */
bool microsd_driver_open(FIL *file, const char *filename, BYTE mode) {
    if (!g_sd_state.initialized || !file || !filename) {
        return false;
    }

    FRESULT fr = f_open(file, filename, mode);
    if (fr != FR_OK) {
        printf("✗ ERROR: Failed to open file '%s' (error %d)\n", filename, fr);
        g_sd_state.last_error = fr;
        return false;
    }

    return true;
}

/*!
 * @brief Close an open file
 * @param file Pointer to FIL structure
 * @return true on success, false on failure
 */
bool microsd_driver_close(FIL *file) {
    if (!file) {
        return false;
    }

    FRESULT fr = f_close(file);
    if (fr != FR_OK) {
        printf("✗ ERROR: Failed to close file (error %d)\n", fr);
        g_sd_state.last_error = fr;
        return false;
    }

    return true;
}

/*!
 * @brief Read data from an open file
 * @param file Pointer to FIL structure
 * @param buffer Buffer to store read data
 * @param bytes_to_read Number of bytes to read
 * @param bytes_read Output: actual bytes read
 * @return true on success, false on failure
 */
bool microsd_driver_read(FIL *file, void *buffer, UINT bytes_to_read, UINT *bytes_read) {
    if (!file || !buffer) {
        return false;
    }

    FRESULT fr = f_read(file, buffer, bytes_to_read, bytes_read);
    if (fr != FR_OK) {
        printf("✗ ERROR: Failed to read file (error %d)\n", fr);
        g_sd_state.last_error = fr;
        return false;
    }

    g_sd_state.total_reads++;
    return true;
}

/*!
 * @brief Write data to an open file
 * @param file Pointer to FIL structure
 * @param buffer Buffer containing data to write
 * @param bytes_to_write Number of bytes to write
 * @param bytes_written Output: actual bytes written
 * @return true on success, false on failure
 */
bool microsd_driver_write(FIL *file, const void *buffer, UINT bytes_to_write, UINT *bytes_written) {
    if (!file || !buffer) {
        return false;
    }

    FRESULT fr = f_write(file, buffer, bytes_to_write, bytes_written);
    if (fr != FR_OK) {
        printf("✗ ERROR: Failed to write file (error %d)\n", fr);
        g_sd_state.last_error = fr;
        return false;
    }

    g_sd_state.total_writes++;
    return true;
}

/*!
 * @brief Seek to a position in an open file
 * @param file Pointer to FIL structure
 * @param offset Byte offset from beginning of file
 * @return true on success, false on failure
 */
bool microsd_driver_seek(FIL *file, FSIZE_t offset) {
    if (!file) {
        return false;
    }

    FRESULT fr = f_lseek(file, offset);
    if (fr != FR_OK) {
        printf("✗ ERROR: Failed to seek in file (error %d)\n", fr);
        g_sd_state.last_error = fr;
        return false;
    }

    return true;
}

/*!
 * @brief Sync (flush) file changes to disk
 * @param file Pointer to FIL structure
 * @return true on success, false on failure
 */
bool microsd_driver_sync(FIL *file) {
    if (!file) {
        return false;
    }

    FRESULT fr = f_sync(file);
    if (fr != FR_OK) {
        printf("✗ ERROR: Failed to sync file (error %d)\n", fr);
        g_sd_state.last_error = fr;
        return false;
    }

    return true;
}

/*!
 * @brief Get file size
 * @param file Pointer to FIL structure
 * @return File size in bytes, or 0 on error
 */
FSIZE_t microsd_driver_get_size(FIL *file) {
    if (!file) {
        return 0;
    }

    return f_size(file);
}

/*!
 * @brief Get file information without opening the file
 * @param filename Filename to query
 * @param fileinfo Pointer to FILINFO structure to receive info
 * @return true on success, false on failure
 */
bool microsd_driver_stat(const char *filename, FILINFO *fileinfo) {
    if (!g_sd_state.initialized || !filename || !fileinfo) {
        return false;
    }

    FRESULT fr = f_stat(filename, fileinfo);
    if (fr != FR_OK) {
        g_sd_state.last_error = fr;
        return false;
    }

    return true;
}

/*!
 * @brief Check if a file exists
 * @param filename Filename to check
 * @return true if file exists, false otherwise
 */
bool microsd_driver_file_exists(const char *filename) {
    FILINFO fno;
    return microsd_driver_stat(filename, &fno);
}

/*!
 * @brief Delete a file
 * @param filename Filename to delete
 * @return true on success, false on failure
 */
bool microsd_driver_unlink(const char *filename) {
    if (!g_sd_state.initialized || !filename) {
        return false;
    }

    FRESULT fr = f_unlink(filename);
    if (fr != FR_OK) {
        printf("✗ ERROR: Failed to delete file '%s' (error %d)\n", filename, fr);
        g_sd_state.last_error = fr;
        return false;
    }

    return true;
}

/*!
 * @brief Rename/move a file
 * @param old_name Current filename
 * @param new_name New filename
 * @return true on success, false on failure
 */
bool microsd_driver_rename(const char *old_name, const char *new_name) {
    if (!g_sd_state.initialized || !old_name || !new_name) {
        return false;
    }

    FRESULT fr = f_rename(old_name, new_name);
    if (fr != FR_OK) {
        printf("✗ ERROR: Failed to rename '%s' to '%s' (error %d)\n", old_name, new_name, fr);
        g_sd_state.last_error = fr;
        return false;
    }

    return true;
}

/*!
 * @brief Create a directory
 * @param dirname Directory name to create
 * @return true on success, false on failure
 */
bool microsd_driver_mkdir(const char *dirname) {
    if (!g_sd_state.initialized || !dirname) {
        return false;
    }

    FRESULT fr = f_mkdir(dirname);
    if (fr != FR_OK && fr != FR_EXIST) {
        printf("✗ ERROR: Failed to create directory '%s' (error %d)\n", dirname, fr);
        g_sd_state.last_error = fr;
        return false;
    }

    return true;
}

/*!
 * @brief Open a directory for reading
 * @param dir Pointer to DIR structure
 * @param dirname Directory name to open
 * @return true on success, false on failure
 */
bool microsd_driver_opendir(DIR *dir, const char *dirname) {
    if (!g_sd_state.initialized || !dir || !dirname) {
        return false;
    }

    FRESULT fr = f_opendir(dir, dirname);
    if (fr != FR_OK) {
        printf("✗ ERROR: Failed to open directory '%s' (error %d)\n", dirname, fr);
        g_sd_state.last_error = fr;
        return false;
    }

    return true;
}

/*!
 * @brief Read next directory entry
 * @param dir Pointer to DIR structure
 * @param fileinfo Pointer to FILINFO structure to receive entry info
 * @return true on success, false on error or end of directory
 */
bool microsd_driver_readdir(DIR *dir, FILINFO *fileinfo) {
    if (!dir || !fileinfo) {
        return false;
    }

    FRESULT fr = f_readdir(dir, fileinfo);
    if (fr != FR_OK) {
        g_sd_state.last_error = fr;
        return false;
    }

    // Return false at end of directory (empty filename)
    return (fileinfo->fname[0] != '\0');
}

/*!
 * @brief Close a directory
 * @param dir Pointer to DIR structure
 * @return true on success, false on failure
 */
bool microsd_driver_closedir(DIR *dir) {
    if (!dir) {
        return false;
    }

    FRESULT fr = f_closedir(dir);
    if (fr != FR_OK) {
        printf("✗ ERROR: Failed to close directory (error %d)\n", fr);
        g_sd_state.last_error = fr;
        return false;
    }

    return true;
}

/*!
 * @brief List all files in a directory
 * @param dirname Directory name (use "" or "/" for root)
 * @return true on success, false on failure
 */
bool microsd_driver_list_directory(const char *dirname) {
    if (!g_sd_state.initialized) {
        return false;
    }

    DIR dir;
    FILINFO fno;

    if (!microsd_driver_opendir(&dir, dirname)) {
        return false;
    }

    printf("\n=== Directory listing: %s ===\n", dirname);

    while (microsd_driver_readdir(&dir, &fno)) {
        if (fno.fattrib & AM_DIR) {
            printf("  [DIR]  %s\n", fno.fname);
        } else {
            printf("  [FILE] %s (%lu bytes)\n", fno.fname, (unsigned long)fno.fsize);
        }
    }

    printf("=== End of directory ===\n\n");

    microsd_driver_closedir(&dir);
    return true;
}

/*!
 * @brief Read entire file into buffer (convenience function)
 * @param filename Filename to read
 * @param buffer Buffer to store file contents
 * @param buffer_size Size of buffer
 * @param bytes_read Output: actual bytes read
 * @return true on success, false on failure
 */
bool microsd_driver_read_file(const char *filename, void *buffer, size_t buffer_size,
                              size_t *bytes_read) {
    if (!filename || !buffer || !bytes_read) {
        return false;
    }

    FIL file;
    if (!microsd_driver_open(&file, filename, FA_READ)) {
        return false;
    }

    UINT read = 0;
    bool success = microsd_driver_read(&file, buffer, buffer_size, &read);
    *bytes_read = read;

    microsd_driver_close(&file);
    return success;
}

/*!
 * @brief Write entire buffer to file (convenience function)
 * @param filename Filename to write
 * @param buffer Buffer containing data to write
 * @param size Number of bytes to write
 * @return true on success, false on failure
 */
bool microsd_driver_write_file(const char *filename, const void *buffer, size_t size) {
    if (!filename || !buffer) {
        return false;
    }

    FIL file;
    if (!microsd_driver_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS)) {
        return false;
    }

    UINT written = 0;
    bool success = microsd_driver_write(&file, buffer, size, &written);

    microsd_driver_close(&file);
    return success && (written == size);
}

/*!
 * @brief Read file with optimized buffering (32KB chunks)
 * @param filename Filename to read
 * @param buffer Buffer to store file contents
 * @param buffer_size Size of buffer
 * @param bytes_read Output: actual bytes read
 * @return true on success, false on failure
 */
bool microsd_driver_read_file_buffered(const char *filename, void *buffer, size_t buffer_size,
                                       size_t *bytes_read) {
    if (!filename || !buffer || !bytes_read) {
        return false;
    }

    FIL file;
    if (!microsd_driver_open(&file, filename, FA_READ)) {
        return false;
    }

    size_t total_read = 0;
    uint8_t *dest = (uint8_t *)buffer;

    while (total_read < buffer_size) {
        size_t chunk_size = (buffer_size - total_read);
        if (chunk_size > READ_BUFFER_SIZE) {
            chunk_size = READ_BUFFER_SIZE;
        }

        UINT read = 0;
        if (!microsd_driver_read(&file, g_read_buffer, chunk_size, &read)) {
            microsd_driver_close(&file);
            return false;
        }

        if (read == 0) {
            break; // EOF
        }

        memcpy(dest + total_read, g_read_buffer, read);
        total_read += read;
    }

    *bytes_read = total_read;
    microsd_driver_close(&file);
    return true;
}

/*!
 * @brief Get driver statistics
 * @param total_reads Output: total read operations
 * @param total_writes Output: total write operations
 * @param last_error Output: last error code
 */
void microsd_driver_get_stats(uint32_t *total_reads, uint32_t *total_writes, uint32_t *last_error) {
    if (total_reads) {
        *total_reads = g_sd_state.total_reads;
    }
    if (total_writes) {
        *total_writes = g_sd_state.total_writes;
    }
    if (last_error) {
        *last_error = g_sd_state.last_error;
    }
}

/*!
 * @brief Print driver information
 */
void microsd_driver_print_info(void) {
    printf("\n=== MicroSD Driver Info ===\n");
    printf("Driver ready: %s\n", g_sd_state.driver_ready ? "Yes" : "No");
    printf("Filesystem mounted: %s\n", g_sd_state.initialized ? "Yes" : "No");
    printf("Total reads: %lu\n", (unsigned long)g_sd_state.total_reads);
    printf("Total writes: %lu\n", (unsigned long)g_sd_state.total_writes);
    printf("Last error: %lu\n", (unsigned long)g_sd_state.last_error);

    if (g_sd_state.initialized) {
        uint64_t total, free;
        if (microsd_driver_get_space(&total, &free)) {
            printf("Total space: %llu bytes (%.2f MB)\n", total, total / (1024.0 * 1024.0));
            printf("Free space: %llu bytes (%.2f MB)\n", free, free / (1024.0 * 1024.0));
            printf("Used space: %llu bytes (%.2f MB)\n", total - free,
                   (total - free) / (1024.0 * 1024.0));
        }
    }

    printf("===========================\n\n");
}

/*!
 * @brief Get FatFS error description
 * @param error_code FatFS error code
 * @return String description of error
 */
const char *microsd_driver_get_error_string(FRESULT error_code) {
    switch (error_code) {
    case FR_OK:
        return "Success";
    case FR_DISK_ERR:
        return "Disk error";
    case FR_INT_ERR:
        return "Internal error";
    case FR_NOT_READY:
        return "Drive not ready";
    case FR_NO_FILE:
        return "File not found";
    case FR_NO_PATH:
        return "Path not found";
    case FR_INVALID_NAME:
        return "Invalid name";
    case FR_DENIED:
        return "Access denied";
    case FR_EXIST:
        return "File exists";
    case FR_INVALID_OBJECT:
        return "Invalid object";
    case FR_WRITE_PROTECTED:
        return "Write protected";
    case FR_INVALID_DRIVE:
        return "Invalid drive";
    case FR_NOT_ENABLED:
        return "Not enabled";
    case FR_NO_FILESYSTEM:
        return "No filesystem";
    case FR_MKFS_ABORTED:
        return "Format aborted";
    case FR_TIMEOUT:
        return "Timeout";
    case FR_LOCKED:
        return "File locked";
    case FR_NOT_ENOUGH_CORE:
        return "Not enough memory";
    case FR_TOO_MANY_OPEN_FILES:
        return "Too many open files";
    case FR_INVALID_PARAMETER:
        return "Invalid parameter";
    default:
        return "Unknown error";
    }
}
