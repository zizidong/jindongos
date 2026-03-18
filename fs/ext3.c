#include "ext3.h"

static ext3_superblock_t superblock;

void ext3_init(void) {
    (void)superblock;
    // Read superblock from disk
    // Validate magic number
}

ext3_inode_t* ext3_read_inode(uint32_t inode_num) {
    (void)inode_num;
    static ext3_inode_t inode;
    // Calculate inode location
    // Read inode from disk
    return &inode;
}

uint32_t ext3_read_block(uint32_t block_num, void *buffer) {
    (void)block_num;
    (void)buffer;
    // Read block from disk
    return 0;
}
