/*!
 * @file    microsd_exfat.c
 * @brief   exFAT filesystem operations for microSD driver
 * @author  INF2004 Project Team
 * @date    2025
 */

#include <stdio.h>
#include <string.h>

#include "microsd_internal.h"

/* Forward declarations for helper functions */
static bool is_cluster_free_in_bitmap(filesystem_info_t const *const p_fs_info, uint32_t cluster,
                                      uint32_t bitmap_cluster);
static uint32_t find_allocation_bitmap_cluster(filesystem_info_t const *const p_fs_info);

uint32_t microsd_cluster_to_sector(filesystem_info_t const *const p_fs_info, uint32_t cluster) {
    return p_fs_info->partition_offset + p_fs_info->cluster_heap_offset +
           (cluster - 2) * p_fs_info->sectors_per_cluster;
}

void microsd_get_fat_entry_location(filesystem_info_t const *const p_fs_info, uint32_t cluster,
                                    uint32_t *fat_sector, uint32_t *entry_offset) {
    /* Each FAT entry is 4 bytes (32 bits) in exFAT */
    uint32_t byte_offset = cluster * sizeof(uint32_t);

    /* Calculate which sector contains this FAT entry */
    *fat_sector =
        p_fs_info->partition_offset + p_fs_info->fat_offset + (byte_offset / SD_BLOCK_SIZE);

    /* Calculate byte offset within that sector */
    *entry_offset = byte_offset % SD_BLOCK_SIZE;
}

uint32_t microsd_get_next_cluster(filesystem_info_t const *const p_fs_info, uint32_t cluster) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t fat_sector, fat_offset;
    uint32_t next_cluster;

    /* Calculate FAT sector and offset using centralized function */
    microsd_get_fat_entry_location(p_fs_info, cluster, &fat_sector, &fat_offset);

    /* Read FAT sector */
    if (!microsd_read_block(fat_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector\n");
        return 0xFFFFFFFF;
    }

    /* Extract next cluster value */
    next_cluster = (uint32_t)buffer[fat_offset] | ((uint32_t)buffer[fat_offset + 1] << 8) |
                   ((uint32_t)buffer[fat_offset + 2] << 16) |
                   ((uint32_t)buffer[fat_offset + 3] << 24);

    /* Check for end of chain marker */
    if (next_cluster >= 0xFFFFFFF8) {
        return 0xFFFFFFFF; /* End of chain */
    }

    return next_cluster;
}

static bool is_cluster_free_in_bitmap(filesystem_info_t const *const p_fs_info, uint32_t cluster,
                                      uint32_t bitmap_cluster) {
    uint8_t buffer[SD_BLOCK_SIZE];

    /* Calculate which byte and bit within the bitmap (clusters start from index 2) */
    uint32_t bit_index = cluster - 2;
    uint32_t byte_offset = bit_index / 8;
    uint8_t bit_offset = bit_index % 8;

    /* Calculate which sector contains this byte */
    uint32_t bitmap_sector = microsd_cluster_to_sector(p_fs_info, bitmap_cluster);
    uint32_t sector_offset = byte_offset / SD_BLOCK_SIZE;
    uint32_t byte_in_sector = byte_offset % SD_BLOCK_SIZE;

    /* Read the bitmap sector */
    if (!microsd_read_block(bitmap_sector + sector_offset, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read allocation bitmap sector\n");
        return false; /* Assume allocated if we can't read */
    }

    /* Check if the bit is 0 (free) or 1 (allocated) */
    bool is_allocated = (buffer[byte_in_sector] & (1 << bit_offset)) != 0;
    return !is_allocated;
}

static uint32_t find_allocation_bitmap_cluster(filesystem_info_t const *const p_fs_info) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t root_sector = microsd_cluster_to_sector(p_fs_info, p_fs_info->root_cluster);

    /* Read root directory to find allocation bitmap directory entry */
    if (!microsd_read_block(root_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read root directory for bitmap search\n");
        return 0;
    }

    /* Search for allocation bitmap directory entry (TypeCode = 1, TypeImportance = 0) */
    for (int i = 0; i < SD_BLOCK_SIZE; i += 32) {
        uint8_t entry_type = buffer[i];

        /* Check if this is an allocation bitmap directory entry */
        if (entry_type == 0x81) { /* 0x81 = TypeCode=1, TypeImportance=0, TypeCategory=0, InUse=1 */
            /* Extract FirstCluster from bytes 20-23 */
            uint32_t bitmap_cluster = *(uint32_t *)&buffer[i + 20];
            return bitmap_cluster;
        }

        /* Stop at end-of-directory marker */
        if (entry_type == 0x00) {
            break;
        }
    }

    return 0;
}

uint32_t microsd_find_free_cluster(filesystem_info_t const *const p_fs_info) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t fat_sector = p_fs_info->partition_offset + p_fs_info->fat_offset;
    uint32_t entries_per_sector = SD_BLOCK_SIZE / sizeof(uint32_t);

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Searching for free cluster in FAT (partition offset: %lu)\n",
                (unsigned long)p_fs_info->partition_offset);

    /* Find allocation bitmap cluster */
    uint32_t bitmap_cluster = find_allocation_bitmap_cluster(p_fs_info);
    if (bitmap_cluster == 0) {
        MICROSD_LOG(MICROSD_LOG_WARN, "Allocation bitmap not found, using FAT only\n");
    }

    /* Search through FAT sectors */
    for (uint32_t sector = 0; sector < p_fs_info->fat_length; sector++) {
        if (!microsd_read_block(fat_sector + sector, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector %lu\n",
                        (unsigned long)(fat_sector + sector));
            continue;
        }

        uint32_t *fat_entries = (uint32_t *)buffer;

        /* Debug: Show first few FAT entries to understand allocation */
        if (sector == 0) {
            MICROSD_LOG(MICROSD_LOG_DEBUG, "FAT entries for clusters 2-7: ");
            for (uint32_t i = 0; i < 6 && i < entries_per_sector; i++) {
                MICROSD_LOG(MICROSD_LOG_DEBUG, "cluster %lu=0x%08lX ", (unsigned long)(i + 2),
                            (unsigned long)fat_entries[i]);
            }
            MICROSD_LOG(MICROSD_LOG_DEBUG, "\n");
        }

        for (uint32_t entry = 0; entry < entries_per_sector; entry++) {
            uint32_t cluster = sector * entries_per_sector + entry + 2; /* Clusters start at 2 */

            if (cluster >= p_fs_info->cluster_count + 2) {
                break; /* Beyond valid cluster range */
            }

            /* Check if cluster is free in FAT */
            if (fat_entries[entry] == 0) {
                /* Also check allocation bitmap if available */
                if (bitmap_cluster != 0) {
                    if (is_cluster_free_in_bitmap(p_fs_info, cluster, bitmap_cluster)) {
                        MICROSD_LOG(MICROSD_LOG_DEBUG,
                                    "Found free cluster: %lu (FAT and bitmap both show free)\n",
                                    (unsigned long)cluster);
                        return cluster;
                    } else {
                        MICROSD_LOG(MICROSD_LOG_DEBUG,
                                    "Cluster %lu free in FAT but allocated in bitmap, skipping\n",
                                    (unsigned long)cluster);
                    }
                } else {
                    /* No bitmap available, trust FAT */
                    MICROSD_LOG(MICROSD_LOG_DEBUG, "Found free cluster: %lu\n",
                                (unsigned long)cluster);
                    return cluster;
                }
            }
        }
    }

    MICROSD_LOG(MICROSD_LOG_ERROR, "No free clusters found\n");
    return 0;
}

bool microsd_mark_cluster_end(filesystem_info_t const *const p_fs_info, uint32_t cluster) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t fat_sector, entry_offset;

    microsd_get_fat_entry_location(p_fs_info, cluster, &fat_sector, &entry_offset);

    /* Read FAT sector */
    if (!microsd_read_block(fat_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector %lu\n",
                    (unsigned long)fat_sector);
        return false;
    }

    /* Mark as end-of-chain (0xFFFFFFFF) */
    uint32_t *fat_entry = (uint32_t *)(buffer + entry_offset);
    *fat_entry = 0xFFFFFFFF;

    /* Write back */
    if (!microsd_write_block(fat_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write FAT sector %lu\n",
                    (unsigned long)fat_sector);
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Marked cluster %lu as end-of-chain\n", (unsigned long)cluster);
    return true;
}

bool microsd_update_allocation_bitmap(filesystem_info_t const *const p_fs_info, uint32_t cluster) {
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t bitmap_cluster = 0;

    /* Find allocation bitmap by reading root directory entries */
    uint32_t root_sector = microsd_cluster_to_sector(p_fs_info, p_fs_info->root_cluster);

    /* Read root directory to find allocation bitmap directory entry */
    if (!microsd_read_block(root_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read root directory\n");
        return false;
    }

    /* Debug: Print all directory entries in root */
    MICROSD_LOG(MICROSD_LOG_INFO, "=== Root Directory Entries ===\n");
    for (int i = 0; i < SD_BLOCK_SIZE; i += 32) {
        uint8_t entry_type = buffer[i];
        if (entry_type == 0x00) {
            MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: End-of-directory marker\n", i / 32);
            break;
        } else if (entry_type != 0xFF) {
            MICROSD_LOG(MICROSD_LOG_INFO, "Entry %d: Type=0x%02X", i / 32, entry_type);
            if (entry_type & 0x80) {
                uint8_t type_code = entry_type & 0x1F;
                uint8_t type_importance = (entry_type >> 5) & 0x01;
                uint8_t type_category = (entry_type >> 6) & 0x01;
                MICROSD_LOG(MICROSD_LOG_INFO, " (TypeCode=%d, Importance=%d, Category=%d)\n",
                            type_code, type_importance, type_category);

                /* Print FirstCluster and DataLength ONLY for specific entry types */
                if (entry_type == 0x81 || entry_type == 0x82) {
                    /* Allocation Bitmap (0x81) or Up-case Table (0x82) - have FirstCluster */
                    uint32_t first_cluster = *(uint32_t *)&buffer[i + 20];
                    uint64_t data_length = *(uint64_t *)&buffer[i + 24];
                    MICROSD_LOG(MICROSD_LOG_INFO, "        FirstCluster=%u, DataLength=%llu\n",
                                first_cluster, (unsigned long long)data_length);
                } else if (entry_type == 0xC0) {
                    /* Stream Extension - contains file data information */
                    uint32_t first_cluster = *(uint32_t *)&buffer[i + 20];
                    uint64_t data_length = *(uint64_t *)&buffer[i + 24];
                    MICROSD_LOG(MICROSD_LOG_INFO, "        FirstCluster=%u, DataLength=%llu\n",
                                first_cluster, (unsigned long long)data_length);
                }
                /* File Entry (0x85) doesn't have FirstCluster/DataLength, skip */
            } else {
                MICROSD_LOG(MICROSD_LOG_INFO, " (Unused)\n");
            }
        }
    }
    MICROSD_LOG(MICROSD_LOG_INFO, "=== End Directory Entries ===\n");

    /* Search for allocation bitmap directory entry (TypeCode = 1, TypeImportance = 0) */
    for (int i = 0; i < SD_BLOCK_SIZE; i += 32) {
        uint8_t entry_type = buffer[i];

        /* Check if this is an allocation bitmap directory entry */
        if (entry_type == 0x81) {
            /* Extract FirstCluster from bytes 20-23 */
            bitmap_cluster = *(uint32_t *)&buffer[i + 20];
            MICROSD_LOG(MICROSD_LOG_INFO, "Found allocation bitmap at cluster %u\n",
                        bitmap_cluster);
            break;
        }

        /* Stop at end-of-directory marker */
        if (entry_type == 0x00) {
            break;
        }
    }

    if (bitmap_cluster == 0) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Could not find allocation bitmap directory entry\n");

        /* Check if root directory is completely empty (needs initialization) */
        bool directory_empty = true;
        for (int i = 0; i < SD_BLOCK_SIZE; i += 32) {
            if (buffer[i] != 0x00) {
                directory_empty = false;
                break;
            }
        }

        if (directory_empty) {
            MICROSD_LOG(MICROSD_LOG_ERROR,
                        "Root directory is empty - this indicates an improperly formatted exFAT "
                        "filesystem\n");
            MICROSD_LOG(MICROSD_LOG_ERROR,
                        "Please format the SD card with a proper exFAT filesystem before use\n");
            return false;
        }
    }

    /* Now read the actual allocation bitmap */
    uint32_t bitmap_sector = microsd_cluster_to_sector(p_fs_info, bitmap_cluster);

    /* Calculate which byte and bit within the bitmap (clusters start from index 2) */
    uint32_t bit_index = cluster - 2;
    uint32_t byte_offset = bit_index / 8;
    uint8_t bit_offset = bit_index % 8;

    /* Calculate which sector contains this byte */
    uint32_t sector_offset = byte_offset / SD_BLOCK_SIZE;
    uint32_t byte_in_sector = byte_offset % SD_BLOCK_SIZE;

    /* Read the bitmap sector */
    if (!microsd_read_block(bitmap_sector + sector_offset, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read allocation bitmap sector\n");
        return false;
    }

    /* Set the bit for this cluster */
    buffer[byte_in_sector] |= (1 << bit_offset);

    /* Write back the bitmap sector */
    if (!microsd_write_block(bitmap_sector + sector_offset, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write allocation bitmap sector\n");
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Updated allocation bitmap: cluster %lu marked as used\n",
                (unsigned long)cluster);
    return true;
}

bool microsd_allocate_cluster_chain(filesystem_info_t const *const p_fs_info, uint32_t data_length,
                                    uint32_t *const p_first_cluster) {
    uint32_t bytes_per_cluster = p_fs_info->sectors_per_cluster * SD_BLOCK_SIZE;
    uint32_t clusters_needed = (data_length + bytes_per_cluster - 1) / bytes_per_cluster;
    uint32_t first_cluster = 0;
    uint32_t prev_cluster = 0;

    if (NULL == p_first_cluster) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid parameter for cluster allocation\n");
        return false;
    }

    if (clusters_needed == 0) {
        clusters_needed = 1; /* Minimum one cluster */
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Allocating cluster chain for %lu bytes (%lu clusters)\n",
                (unsigned long)data_length, (unsigned long)clusters_needed);

    /* Allocate clusters and link them together */
    for (uint32_t i = 0; i < clusters_needed; i++) {
        uint32_t cluster = microsd_find_free_cluster(p_fs_info);
        if (cluster == 0) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to find free cluster %lu of %lu\n",
                        (unsigned long)(i + 1), (unsigned long)clusters_needed);
            return false;
        }

        if (i == 0) {
            first_cluster = cluster;
        }

        /* Link previous cluster to this one */
        if (prev_cluster != 0) {
            uint8_t buffer[SD_BLOCK_SIZE];
            uint32_t fat_sector, entry_offset;
            microsd_get_fat_entry_location(p_fs_info, prev_cluster, &fat_sector, &entry_offset);

            if (!microsd_read_block(fat_sector, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector for linking\n");
                return false;
            }

            uint32_t *fat_entry = (uint32_t *)(buffer + entry_offset);
            *fat_entry = cluster;

            if (!microsd_write_block(fat_sector, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write FAT sector for linking\n");
                return false;
            }

            MICROSD_LOG(MICROSD_LOG_INFO,
                        "Linked cluster %lu -> %lu (FAT sector %lu, offset %lu)\n",
                        (unsigned long)prev_cluster, (unsigned long)cluster,
                        (unsigned long)fat_sector, (unsigned long)entry_offset);
        }

        /* Update allocation bitmap for this cluster */
        if (!microsd_update_allocation_bitmap(p_fs_info, cluster)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to update allocation bitmap for cluster %lu\n",
                        (unsigned long)cluster);
            return false;
        }

        prev_cluster = cluster;
    }

    /* Mark the last cluster as end-of-chain */
    if (!microsd_mark_cluster_end(p_fs_info, prev_cluster)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to mark last cluster as end-of-chain\n");
        return false;
    }

    *p_first_cluster = first_cluster;
    MICROSD_LOG(MICROSD_LOG_INFO,
                "Successfully allocated cluster chain: %lu clusters starting at %lu\n",
                (unsigned long)clusters_needed, (unsigned long)first_cluster);

    return true;
}

bool microsd_expand_directory(filesystem_info_t const *const p_fs_info, uint32_t dir_cluster) {
    uint32_t new_cluster;
    uint8_t buffer[SD_BLOCK_SIZE];

    MICROSD_LOG(MICROSD_LOG_INFO, "Expanding directory cluster %lu\n", (unsigned long)dir_cluster);

    /* Find a free cluster for directory expansion */
    new_cluster = microsd_find_free_cluster(p_fs_info);
    if (new_cluster == 0) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "No free cluster available for directory expansion\n");
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Allocated cluster %lu for directory expansion\n",
                (unsigned long)new_cluster);

    /* Initialize the new directory cluster with zeros */
    memset(buffer, 0, SD_BLOCK_SIZE);
    for (uint32_t sector = 0; sector < p_fs_info->sectors_per_cluster; sector++) {
        uint32_t target_sector = microsd_cluster_to_sector(p_fs_info, new_cluster) + sector;
        if (!microsd_write_block(target_sector, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR,
                        "Failed to initialize new directory cluster sector %lu\n",
                        (unsigned long)target_sector);
            return false;
        }
    }

    /* Find the last cluster in the directory chain */
    uint32_t current_cluster = dir_cluster;
    uint32_t last_cluster = dir_cluster;

    while (current_cluster != 0xFFFFFFFF) {
        last_cluster = current_cluster;

        /* Read FAT entry for current cluster */
        uint32_t fat_sector, entry_offset;
        microsd_get_fat_entry_location(p_fs_info, current_cluster, &fat_sector, &entry_offset);

        if (!microsd_read_block(fat_sector, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector %lu\n",
                        (unsigned long)fat_sector);
            return false;
        }

        uint32_t *fat_entry = (uint32_t *)(buffer + entry_offset);
        current_cluster = *fat_entry;

        /* If this is the end-of-chain, we found our last cluster */
        if (current_cluster == 0xFFFFFFFF) {
            break;
        }
    }

    MICROSD_LOG(MICROSD_LOG_DEBUG, "Last cluster in directory chain: %lu\n",
                (unsigned long)last_cluster);

    /* Link the last cluster to the new cluster */
    uint32_t fat_sector, entry_offset;
    microsd_get_fat_entry_location(p_fs_info, last_cluster, &fat_sector, &entry_offset);

    if (!microsd_read_block(fat_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read FAT sector %lu for linking\n",
                    (unsigned long)fat_sector);
        return false;
    }

    uint32_t *fat_entry = (uint32_t *)(buffer + entry_offset);
    *fat_entry = new_cluster;

    if (!microsd_write_block(fat_sector, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write FAT sector %lu for linking\n",
                    (unsigned long)fat_sector);
        return false;
    }

    /* Mark the new cluster as end-of-chain */
    if (!microsd_mark_cluster_end(p_fs_info, new_cluster)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to mark new cluster %lu as end-of-chain\n",
                    (unsigned long)new_cluster);
        return false;
    }

    /* Update allocation bitmap for the new cluster */
    if (!microsd_update_allocation_bitmap(p_fs_info, new_cluster)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to update allocation bitmap for cluster %lu\n",
                    (unsigned long)new_cluster);
        return false;
    }

    MICROSD_LOG(MICROSD_LOG_INFO, "Successfully expanded directory: linked cluster %lu -> %lu\n",
                (unsigned long)last_cluster, (unsigned long)new_cluster);

    return true;
}

bool microsd_init_filesystem(filesystem_info_t *const p_fs_info) {
    uint8_t buffer[SD_BLOCK_SIZE];
    mbr_t *mbr;
    exfat_boot_sector_t *boot_sector;
    uint32_t partition_start = 0;

    if (NULL == p_fs_info) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid filesystem info pointer\n");
        return false;
    }

    /* Initialize partition offset */
    p_fs_info->partition_offset = 0;

    /* Read block 0 (MBR or boot sector) */
    if (!microsd_read_block(0, buffer)) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read block 0\n");
        return false;
    }

    /* Check boot sector signature first */
    if (buffer[510] != 0x55 || buffer[511] != 0xAA) {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Invalid boot sector signature\n");
        return false;
    }

    /* Debug: Show what we read from block 0 */
    MICROSD_LOG(MICROSD_LOG_DEBUG, "Block 0 signature: %02X %02X\n", buffer[510], buffer[511]);

    /* Hex dump first 64 bytes */
    MICROSD_LOG(MICROSD_LOG_DEBUG, "Block 0 hex dump (first 64 bytes):\n");
    for (int i = 0; i < 64; i += 16) {
        MICROSD_LOG(MICROSD_LOG_DEBUG,
                    "%04X: %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X "
                    "%02X %02X\n",
                    i, buffer[i + 0], buffer[i + 1], buffer[i + 2], buffer[i + 3], buffer[i + 4],
                    buffer[i + 5], buffer[i + 6], buffer[i + 7], buffer[i + 8], buffer[i + 9],
                    buffer[i + 10], buffer[i + 11], buffer[i + 12], buffer[i + 13], buffer[i + 14],
                    buffer[i + 15]);
    }

    /* Check if this is a direct filesystem boot sector (no partition table) */
    if (memcmp(&buffer[3], "EXFAT   ", 8) == 0) {
        MICROSD_LOG(MICROSD_LOG_INFO, "Direct exFAT filesystem detected (no partition table)\n");
        partition_start = 0;
    } else if (memcmp(&buffer[82], "FAT32   ", 8) == 0 || memcmp(&buffer[54], "FAT16   ", 8) == 0) {
        MICROSD_LOG(MICROSD_LOG_INFO, "Direct FAT filesystem detected (not exFAT)\n");
        return false;
    } else {
        /* This is likely an MBR with partition table */
        MICROSD_LOG(MICROSD_LOG_INFO, "MBR with partition table detected\n");

        mbr = (mbr_t *)buffer;

        /* Look for exFAT partition (type 0x07) */
        bool partition_found = false;
        for (int i = 0; i < 4; i++) {
            MICROSD_LOG(MICROSD_LOG_DEBUG, "Partition %d: type=0x%02X, first_lba=%lu, size=%lu\n",
                        i, mbr->partitions[i].partition_type,
                        (unsigned long)mbr->partitions[i].first_lba,
                        (unsigned long)mbr->partitions[i].sector_count);

            if (mbr->partitions[i].partition_type == 0x07 && mbr->partitions[i].sector_count > 0) {
                partition_start = mbr->partitions[i].first_lba;
                partition_found = true;
                MICROSD_LOG(MICROSD_LOG_INFO, "Found exFAT partition at sector %lu\n",
                            (unsigned long)partition_start);
                break;
            }
        }

        if (!partition_found) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "No exFAT partition found in MBR\n");
            return false;
        }

        /* Read the actual boot sector from the partition */
        if (!microsd_read_block(partition_start, buffer)) {
            MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to read partition boot sector at sector %lu\n",
                        (unsigned long)partition_start);
            return false;
        }

        MICROSD_LOG(MICROSD_LOG_DEBUG, "Partition boot sector hex dump (first 32 bytes):\n");
        for (int i = 0; i < 32; i += 16) {
            MICROSD_LOG(MICROSD_LOG_DEBUG,
                        "%04X: %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X "
                        "%02X %02X %02X\n",
                        i, buffer[i + 0], buffer[i + 1], buffer[i + 2], buffer[i + 3],
                        buffer[i + 4], buffer[i + 5], buffer[i + 6], buffer[i + 7], buffer[i + 8],
                        buffer[i + 9], buffer[i + 10], buffer[i + 11], buffer[i + 12],
                        buffer[i + 13], buffer[i + 14], buffer[i + 15]);
        }
    }

    /* Now parse the exFAT boot sector */
    boot_sector = (exfat_boot_sector_t *)buffer;

    /* Check if it's exFAT filesystem */
    if (memcmp(boot_sector->filesystem_name, "EXFAT   ", 8) == 0) {
        MICROSD_LOG(MICROSD_LOG_INFO, "exFAT filesystem confirmed in partition\n");

        p_fs_info->is_exfat = true;
        p_fs_info->partition_offset = partition_start;

        /* Debug: Check volume flags for dirty bit and other states (AFTER setting partition_offset)
         */
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Volume flags: 0x%04X\n", boot_sector->volume_flags);
        if (boot_sector->volume_flags & 0x0002) {
            MICROSD_LOG(MICROSD_LOG_INFO, "Volume dirty bit is set - clearing it\n");
            boot_sector->volume_flags &= ~0x0002; /* Clear dirty bit */

            /* Write back the corrected boot sector to the CORRECT partition location */
            if (!microsd_write_block(p_fs_info->partition_offset, buffer)) {
                MICROSD_LOG(MICROSD_LOG_ERROR, "Failed to write corrected boot sector\n");
            } else {
                MICROSD_LOG(MICROSD_LOG_INFO, "Volume dirty bit cleared successfully\n");
            }
        }
        p_fs_info->bytes_per_sector = 1U << boot_sector->bytes_per_sector_shift;
        p_fs_info->sectors_per_cluster = 1U << boot_sector->sectors_per_cluster_shift;
        p_fs_info->bytes_per_cluster = p_fs_info->bytes_per_sector * p_fs_info->sectors_per_cluster;
        p_fs_info->fat_offset = boot_sector->fat_offset;
        p_fs_info->fat_length = boot_sector->fat_length;
        p_fs_info->cluster_heap_offset = boot_sector->cluster_heap_offset;
        p_fs_info->cluster_count = boot_sector->cluster_count;
        p_fs_info->root_cluster = boot_sector->first_cluster_of_root;

        MICROSD_LOG(MICROSD_LOG_DEBUG, "Partition offset: %lu\n",
                    (unsigned long)p_fs_info->partition_offset);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Bytes per sector: %lu\n",
                    (unsigned long)p_fs_info->bytes_per_sector);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Sectors per cluster: %lu\n",
                    (unsigned long)p_fs_info->sectors_per_cluster);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Bytes per cluster: %lu\n",
                    (unsigned long)p_fs_info->bytes_per_cluster);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "FAT offset: %lu\n", (unsigned long)p_fs_info->fat_offset);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Cluster heap offset: %lu\n",
                    (unsigned long)p_fs_info->cluster_heap_offset);
        MICROSD_LOG(MICROSD_LOG_DEBUG, "Root cluster: %lu\n",
                    (unsigned long)p_fs_info->root_cluster);

        return true;
    } else {
        MICROSD_LOG(MICROSD_LOG_ERROR, "Partition does not contain exFAT filesystem\n");
        MICROSD_LOG(MICROSD_LOG_DEBUG,
                    "Filesystem name bytes: %02X %02X %02X %02X %02X %02X %02X %02X\n",
                    boot_sector->filesystem_name[0], boot_sector->filesystem_name[1],
                    boot_sector->filesystem_name[2], boot_sector->filesystem_name[3],
                    boot_sector->filesystem_name[4], boot_sector->filesystem_name[5],
                    boot_sector->filesystem_name[6], boot_sector->filesystem_name[7]);
        return false;
    }
}
