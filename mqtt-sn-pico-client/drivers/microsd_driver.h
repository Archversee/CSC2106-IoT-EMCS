/*!
 * @file    microsd_driver.h
 * @brief   High-level MicroSD interface wrapper for no-OS-FatFS library
 * @author  INF2004 Project Team
 * @date    2025
 *
 * This header provides a simplified, high-level interface to the FatFS library
 * for microSD card operations on Raspberry Pi Pico W.
 */

#ifndef MICROSD_DRIVER_H
#define MICROSD_DRIVER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ff.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Initialize the microSD driver and mount the filesystem
 * @return true on success, false on failure
 */
bool microsd_driver_init(void);

/*!
 * @brief Check if microSD card is present and accessible
 * @return true if card is present and mounted, false otherwise
 */
bool microsd_driver_is_present(void);

/*!
 * @brief Unmount the filesystem
 * @return true on success, false on failure
 */
bool microsd_driver_unmount(void);

/*!
 * @brief Get filesystem information
 * @param total_space Output: total space in bytes (can be NULL)
 * @param free_space Output: free space in bytes (can be NULL)
 * @return true on success, false on failure
 */
bool microsd_driver_get_space(uint64_t *total_space, uint64_t *free_space);

/*!
 * @brief Open a file for reading or writing
 * @param file Pointer to FIL structure
 * @param filename Filename to open
 * @param mode Access mode (FA_READ, FA_WRITE, FA_CREATE_ALWAYS, etc.)
 * @return true on success, false on failure
 */
bool microsd_driver_open(FIL *file, const char *filename, BYTE mode);

/*!
 * @brief Close an open file
 * @param file Pointer to FIL structure
 * @return true on success, false on failure
 */
bool microsd_driver_close(FIL *file);

/*!
 * @brief Read data from an open file
 * @param file Pointer to FIL structure
 * @param buffer Buffer to store read data
 * @param bytes_to_read Number of bytes to read
 * @param bytes_read Output: actual bytes read
 * @return true on success, false on failure
 */
bool microsd_driver_read(FIL *file, void *buffer, UINT bytes_to_read, UINT *bytes_read);

/*!
 * @brief Write data to an open file
 * @param file Pointer to FIL structure
 * @param buffer Buffer containing data to write
 * @param bytes_to_write Number of bytes to write
 * @param bytes_written Output: actual bytes written
 * @return true on success, false on failure
 */
bool microsd_driver_write(FIL *file, const void *buffer, UINT bytes_to_write, UINT *bytes_written);

/*!
 * @brief Seek to a position in an open file
 * @param file Pointer to FIL structure
 * @param offset Byte offset from beginning of file
 * @return true on success, false on failure
 */
bool microsd_driver_seek(FIL *file, FSIZE_t offset);

/*!
 * @brief Sync (flush) file changes to disk
 * @param file Pointer to FIL structure
 * @return true on success, false on failure
 */
bool microsd_driver_sync(FIL *file);

/*!
 * @brief Get file size
 * @param file Pointer to FIL structure
 * @return File size in bytes, or 0 on error
 */
FSIZE_t microsd_driver_get_size(FIL *file);

/*!
 * @brief Get file information without opening the file
 * @param filename Filename to query
 * @param fileinfo Pointer to FILINFO structure to receive info
 * @return true on success, false on failure
 */
bool microsd_driver_stat(const char *filename, FILINFO *fileinfo);

/*!
 * @brief Check if a file exists
 * @param filename Filename to check
 * @return true if file exists, false otherwise
 */
bool microsd_driver_file_exists(const char *filename);

/*!
 * @brief Delete a file
 * @param filename Filename to delete
 * @return true on success, false on failure
 */
bool microsd_driver_unlink(const char *filename);

/*!
 * @brief Rename/move a file
 * @param old_name Current filename
 * @param new_name New filename
 * @return true on success, false on failure
 */
bool microsd_driver_rename(const char *old_name, const char *new_name);

/*!
 * @brief Create a directory
 * @param dirname Directory name to create
 * @return true on success, false on failure
 */
bool microsd_driver_mkdir(const char *dirname);

/*!
 * @brief Open a directory for reading
 * @param dir Pointer to DIR structure
 * @param dirname Directory name to open
 * @return true on success, false on failure
 */
bool microsd_driver_opendir(DIR *dir, const char *dirname);

/*!
 * @brief Read next directory entry
 * @param dir Pointer to DIR structure
 * @param fileinfo Pointer to FILINFO structure to receive entry info
 * @return true on success, false on error or end of directory
 */
bool microsd_driver_readdir(DIR *dir, FILINFO *fileinfo);

/*!
 * @brief Close a directory
 * @param dir Pointer to DIR structure
 * @return true on success, false on failure
 */
bool microsd_driver_closedir(DIR *dir);

/*!
 * @brief List all files in a directory
 * @param dirname Directory name (use "" or "/" for root)
 * @return true on success, false on failure
 */
bool microsd_driver_list_directory(const char *dirname);

/*!
 * @brief Read entire file into buffer (convenience function)
 * @param filename Filename to read
 * @param buffer Buffer to store file contents
 * @param buffer_size Size of buffer
 * @param bytes_read Output: actual bytes read
 * @return true on success, false on failure
 */
bool microsd_driver_read_file(const char *filename, void *buffer, size_t buffer_size,
                              size_t *bytes_read);

/*!
 * @brief Write entire buffer to file (convenience function)
 * @param filename Filename to write
 * @param buffer Buffer containing data to write
 * @param size Number of bytes to write
 * @return true on success, false on failure
 */
bool microsd_driver_write_file(const char *filename, const void *buffer, size_t size);

/*!
 * @brief Read file with optimized buffering (32KB chunks)
 * @param filename Filename to read
 * @param buffer Buffer to store file contents
 * @param buffer_size Size of buffer
 * @param bytes_read Output: actual bytes read
 * @return true on success, false on failure
 *
 * @note This function uses a 32KB internal buffer for optimal SD card performance
 */
bool microsd_driver_read_file_buffered(const char *filename, void *buffer, size_t buffer_size,
                                       size_t *bytes_read);

/*!
 * @brief Get driver statistics
 * @param total_reads Output: total read operations (can be NULL)
 * @param total_writes Output: total write operations (can be NULL)
 * @param last_error Output: last error code (can be NULL)
 */
void microsd_driver_get_stats(uint32_t *total_reads, uint32_t *total_writes, uint32_t *last_error);

/*!
 * @brief Print driver information and statistics
 */
void microsd_driver_print_info(void);

/*!
 * @brief Get FatFS error description
 * @param error_code FatFS error code
 * @return String description of error
 */
const char *microsd_driver_get_error_string(FRESULT error_code);

#ifdef __cplusplus
}
#endif

#endif /* MICROSD_DRIVER_H */
