#pragma once

#include "command_table.h"
#include "../IMRSimulator/src/lba.h"

#define BYTE_TO_BLOCK(n) (((n) / SECTOR_SIZE) + !!((n) % SECTOR_SIZE))


typedef struct {
    size_t max_jarea_num;
    unsigned long size;
    unsigned long capacity;
} jarea_t;


typedef struct _jfs_t jfs_t;
typedef struct _jfs_operations jfs_operations;

struct _jfs_operations {
    int (*read)(jfs_t *fs, unsigned long lba, size_t n, int fid);
    int (*write)(jfs_t *fs, unsigned long lba, size_t n, int fid);
};

struct _jfs_t {
    struct disk *d;
    jfs_operations *jfs_op;
    transaction_head_t head;
    jarea_t jarea;
    unsigned long ins_count;
    unsigned long read_ins_count;
    unsigned long write_ins_count;
    unsigned long delete_ins_count;
};

extern jfs_t jfs;

jfs_t *init_jfs(int size);
void end_jfs(jfs_t *fs);

int init_jarea(jarea_t *jarea, unsigned long max_block_size);
int jarea_write(jfs_t *fs, unsigned long lba, size_t n, int fid);
int jarea_read(jfs_t *fs, unsigned long lba, size_t n, int fid);

int jfs_write(jfs_t *fs, unsigned long lba, size_t n, int fid);
int jfs_read(jfs_t *fs, unsigned long lba, size_t n, int fid);
void jfs_check_out(jfs_t *jfs);
void flush_command_table(transaction_head_t *head, struct disk *d, unsigned long offset);
void flush_jarea(jarea_t *jarea);