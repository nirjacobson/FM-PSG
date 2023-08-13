#include "fat32.h"

bool fat32_init(FAT32_FS* fs, uint32_t partition_lba) {
    uint8_t block[SD_BLOCK_SIZE];

    if (!sd_read_block(partition_lba, block))
        return false;

    uint8_t sectors_per_cluster = *(uint8_t*)(block + 0x0D);
    uint16_t reserved_sectors = *(uint16_t*)(block + 0x0E);
    uint8_t fats = *(uint8_t*)(block + 0x10);
    uint32_t sectors_per_fat = *(uint32_t*)(block + 0x24);
    uint32_t root_dir_first_cluster = *(uint32_t*)(block + 0x2C);

    fs->lba_fat = partition_lba + reserved_sectors;
    fs->lba_clusters = fs->lba_fat + (fats * sectors_per_fat);
    fs->sectors_per_cluster = sectors_per_cluster;
    fs->root_dir_first_cluster = root_dir_first_cluster;

    return true;
}

void fat32_root(FAT32_File* file, FAT32_FS* fs) {
    file->fs = fs;
    strcpy(file->name, "ROOT");
    file->attrib = 0b00010000;
    file->first_cluster = fs->root_dir_first_cluster;
    file->size = 0;
}

void fat32_stream(FAT32_FileStream* stream, FAT32_File* file, SD_Block_Cache* block, fat32_stream_callback callback) {
    stream->file = file;
    stream->position = 0;
    stream->cluster = file->first_cluster;
    stream->block_idx = 0;
    stream->callback = callback;
    stream->block = block;
}

bool fat32_stream_next(FAT32_FileStream* stream, void* userData) {

    if (DIRECTORY(stream->file)) {
        while (true) {
            if (stream->cluster == FAT32_NONEXISTENT_CLUSTER)
                return false;

            uint32_t lba = fat32_cluster_lba(stream->file->fs, stream->cluster) + stream->block_idx;

            if (!sd_block_cache_load(stream->block, lba)) {
                return false;
            }

            uint8_t* entry = stream->block->data + (stream->position % SD_BLOCK_SIZE);

            if (FAT32_ENTRY_DELETED(entry))
                return false;

            if (!(FAT32_ENTRY_UNUSED(entry) ||
                    FAT32_ENTRY_LFN(entry) ||
                    FAT32_ENTRY_VOLUMEID(entry) ||
                    FAT32_ENTRY_HIDDEN(entry))) {
                FAT32_File file;
                file.attrib = entry[11];
                file.fs = stream->file->fs;
                memcpy(file.name, entry, sizeof(file.name));
                file.name[sizeof(file.name)-1] = '\0';
                file.first_cluster = FAT32_ENTRY_CLUSTER_HIGH(entry);
                file.first_cluster <<= 16;
                file.first_cluster |= FAT32_ENTRY_CLUSTER_LOW(entry);
                file.size = FAT32_ENTRY_FILE_SIZE(entry);

                stream->callback(stream, &file, 1, userData);
                break;
            }

            fat32_stream_advance(stream, FAT32_DIRENTRY_SIZE);
        }
    } else {

        if (stream->position >= stream->file->size) {
            return false;
        }
        if (stream->cluster == FAT32_NONEXISTENT_CLUSTER) {
            return false;
        }

        uint32_t lba = fat32_cluster_lba(stream->file->fs, stream->cluster) + stream->block_idx;

        if (!sd_block_cache_load(stream->block, lba)) {
            return false;
        }

        uint16_t block_offset = stream->position % SD_BLOCK_SIZE;
        uint8_t* data = stream->block->data + block_offset;
        uint32_t to_end = SD_BLOCK_SIZE - block_offset;
        uint32_t remaining = stream->file->size - stream->position;
        uint32_t length = remaining < to_end ? remaining : to_end;

        stream->callback(stream, data, length, userData);
    }

    return true;
}

void fat32_stream_advance(FAT32_FileStream* stream, size_t items) {
    if (DIRECTORY(stream->file)) {
        stream->position += FAT32_DIRENTRY_SIZE;
    } else {
        stream->position += items;
    }

    if (stream->position % SD_BLOCK_SIZE == 0) {
        stream->block_idx = (stream->block_idx + 1) % stream->file->fs->sectors_per_cluster;
        if (stream->block_idx == 0) {
            stream->cluster = fat32_next_cluster(stream->file->fs, stream->cluster);
        }
    }
}

void fat32_stream_set_position(FAT32_FileStream* stream, uint32_t position) {
    stream->position = position;
    uint32_t block = position / SD_BLOCK_SIZE;

    uint32_t cluster_idx = block / stream->file->fs->sectors_per_cluster;
    stream->block_idx = block % stream->file->fs->sectors_per_cluster;
    stream->cluster = fat32_cluster_number(stream->file, cluster_idx);
}

uint32_t fat32_cluster_number(const FAT32_File* file, uint32_t cluster_idx) {
    uint32_t idx = 0;
    uint32_t cluster = file->first_cluster;

    for (; idx < cluster_idx && cluster != FAT32_NONEXISTENT_CLUSTER; idx++) {
        cluster = fat32_next_cluster(file->fs, cluster);
    }

    return cluster;
}

uint32_t fat32_next_cluster(const FAT32_FS* fs, uint32_t cluster) {
    uint32_t block_idx = (cluster * FAT32_ENTRY_SIZE) / SD_BLOCK_SIZE;
    uint32_t lba = fs->lba_fat + block_idx;
    uint8_t la = cluster % (SD_BLOCK_SIZE / FAT32_ENTRY_SIZE);

    uint32_t next_cluster;
    sd_read_long(lba, la, &next_cluster);

    return next_cluster;
}

uint32_t fat32_cluster_lba(const FAT32_FS* fs, uint32_t cluster) {
    return fs->lba_clusters + (cluster - 2) * fs->sectors_per_cluster;
}

bool fat32_file_has_extension(FAT32_File* file, const char* extension) {
    return strncmp(&file->name[8], extension, strlen(extension)) == 0;
}

bool fat32_get_file_from_directory(FAT32_File* dir, FAT32_File* file, const char* name) {
    uint8_t block[SD_BLOCK_SIZE];
    uint32_t lba =  fat32_cluster_lba(dir->fs, dir->first_cluster);
    sd_read_block(lba, block);

    uint32_t position = 0;

    while (position < SD_BLOCK_SIZE) {
        uint8_t* entry = block + (position % SD_BLOCK_SIZE);

        if (FAT32_ENTRY_DELETED(entry))
            return false;

        if (!(FAT32_ENTRY_UNUSED(entry) ||
                FAT32_ENTRY_LFN(entry) ||
                FAT32_ENTRY_VOLUMEID(entry) ||
                FAT32_ENTRY_HIDDEN(entry))) {
            char filename[12];
            memcpy(filename, entry, sizeof(filename));
            if (strncmp(filename, name, strlen(name)) == 0) {
                file->attrib = entry[11];
                file->fs = dir->fs;
                memcpy(file->name, entry, sizeof(file->name));
                file->name[sizeof(file->name)-1] = '\0';
                file->first_cluster = FAT32_ENTRY_CLUSTER_HIGH(entry);
                file->first_cluster <<= 16;
                file->first_cluster |= FAT32_ENTRY_CLUSTER_LOW(entry);
                file->size = FAT32_ENTRY_FILE_SIZE(entry);
                return true;
            }
        }

        position += FAT32_DIRENTRY_SIZE;
    }

    return false;
}

void fat32_read_block_from_file(FAT32_File* file, uint32_t block, uint8_t* data) {
    uint32_t cluster_idx = block / file->fs->sectors_per_cluster;
    uint32_t cluster = fat32_cluster_number(file, cluster_idx);
    uint8_t block_idx = block % file->fs->sectors_per_cluster;
    uint32_t lba = fat32_cluster_lba(file->fs, cluster) + block_idx;

    sd_read_block(lba, data);
}
