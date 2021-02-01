#pragma once

#include "command_table.h"
#include "../IMRSimulator/src/lba.h"

#define BYTE_TO_BLOCK(n) (((n) / SECTOR_SIZE) + !!((n) % SECTOR_SIZE))


typedef struct {
    size_t max_jarea_num;
    unsigned long jarea_size;
} jarea_t;


typedef struct _jfs_t jfs_t;
typedef struct _jfs_operations jfs_operations;

struct _jfs_operations {
    int (*read)(jfs_t *fs, unsigned long lba, size_t n);
    int (*write)(jfs_t *fs, unsigned long lba, size_t n);
};

struct _jfs_t {
    struct disk *d;
    jfs_operations *jfs_op;
    transaction_head_t head;
    jarea_t jarea;
};

extern jfs_t jfs;

jfs_t *init_jfs(int size);
void end_jfs(jfs_t *fs);

int jarea_write(jfs_t *fs, unsigned long lba, size_t n);
int jarea_read(jfs_t *fs, unsigned long lba, size_t n);

int jfs_write(jfs_t *fs, unsigned long lba, size_t n);
int jfs_read(jfs_t *fs, unsigned long lba, size_t n);
