#ifndef _EXT3_H
#define _EXT3_H

#include <stdint.h>

#define EXT3_MAGIC 0xEF53
#define EXT3_BLOCK_SIZE 4096

typedef struct {
    uint32_t inode_count;
    uint32_t block_count;
    uint32_t reserved_blocks;
    uint32_t free_blocks;
    uint32_t free_inodes;
    uint32_t block_size_log;
    uint32_t fragment_size_log;
    uint32_t blocks_per_group;
    uint32_t fragments_per_group;
    uint32_t inodes_per_group;
    uint32_t mount_time;
    uint32_t write_time;
    uint16_t mount_count;
    uint16_t max_mount_count;
    uint16_t magic;
    uint16_t state;
    uint16_t errors;
    uint16_t minor_revision;
    uint32_t last_check;
    uint32_t check_interval;
    uint32_t creator_os;
    uint32_t revision_level;
    uint16_t default_uid;
    uint16_t default_gid;
} ext3_superblock_t;

typedef struct {
    uint16_t mode;
    uint16_t uid;
    uint32_t size;
    uint32_t atime;
    uint32_t ctime;
    uint32_t mtime;
    uint32_t dtime;
    uint16_t gid;
    uint16_t links_count;
    uint32_t blocks;
    uint32_t flags;
    uint32_t osd1;
    uint32_t block[15];
    uint32_t generation;
    uint32_t file_acl;
    uint32_t dir_acl;
    uint32_t faddr;
    uint32_t osd2[3];
} ext3_inode_t;

void ext3_init(void);
ext3_inode_t* ext3_read_inode(uint32_t inode_num);
uint32_t ext3_read_block(uint32_t block_num, void *buffer);

#endif
