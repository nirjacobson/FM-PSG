#ifndef FAT32_H
#define FAT32_H

#define FAT32_NONEXISTENT_CLUSTER 0xFFFFFFFF
#define FAT32_DIRENTRY_SIZE       32
#define FAT32_ENTRY_SIZE           4
#define DIRECTORY_FLAG            (1 << 4)
#define DIRECTORY(f)              (f->attrib & DIRECTORY_FLAG)

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "sd.h"

#define FAT32_ENTRY_DELETED(e)      (e[0] == 0x00)
#define FAT32_ENTRY_UNUSED(e)       (e[0] == 0xE5)
#define FAT32_ENTRY_CLUSTER_HIGH(e) (*(uint16_t*)(e + 0x14))
#define FAT32_ENTRY_CLUSTER_LOW(e)  (*(uint16_t*)(e + 0x1A))
#define FAT32_ENTRY_FILE_SIZE(e)    (*(uint32_t*)(e + 0x1C))
#define FAT32_ENTRY_LFN(e)          ((e[11] & 0xF) == 0xF)
#define FAT32_ENTRY_HIDDEN(e)       (e[11] & (1 << 1))
#define FAT32_ENTRY_VOLUMEID(e)     (e[11] & (1 << 3))

typedef struct {
    uint32_t lba_fat;
    uint32_t lba_clusters;
    uint8_t sectors_per_cluster;
    uint32_t root_dir_first_cluster;
} FAT32_FS;

typedef struct {
    FAT32_FS* fs;
    char name[12];
    uint8_t attrib;
    uint32_t first_cluster;
    uint32_t size;
} FAT32_File;

bool fat32_init(FAT32_FS* fs, uint32_t partition_lba);
void fat32_root(FAT32_File* file, FAT32_FS* fs);

bool fat32_file_has_extension(FAT32_File* file, const char* extension);

uint32_t fat32_next_cluster(const FAT32_FS* fs, uint32_t cluster);

uint32_t fat32_cluster_number(const FAT32_File* file, uint32_t cluster_idx);
uint32_t fat32_cluster_lba(const FAT32_FS* fs, uint32_t cluster);

bool fat32_get_file_from_directory(FAT32_File* dir, FAT32_File* file, const char* name);
void fat32_read_block_from_file(FAT32_File* file, uint32_t block, uint8_t* data);


#endif // FAT32_H
