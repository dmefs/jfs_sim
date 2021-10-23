#pragma once

#include "command_table.h"
#include "lba.h"

#define BYTE_TO_BLOCK(n) (((n) / SECTOR_SIZE) + !!((n) % SECTOR_SIZE))

typedef struct
{
    size_t max_jarea_num;
    unsigned long size;
    unsigned long capacity;
} jarea_t;

typedef struct _jfs_t jfs_t;
typedef struct _jfs_operations jfs_operations;

struct _jfs_operations
{
    int (*read)(jfs_t* fs, unsigned long lba, size_t n, unsigned long fid);
    int (*write)(jfs_t* fs, unsigned long lba, size_t n, unsigned long fid);
    int (*delete)(jfs_t* fs, unsigned long lba, size_t n, unsigned long fid);
};

struct _jfs_t
{
    struct disk* d;
    jfs_operations* jfs_op;
    transaction_head_t head;
    jarea_t jarea;
    long long ins_count;
    long long read_ins_count;
    long long write_ins_count;
    long long delete_ins_count;
};

extern jfs_t jfs;

jfs_t*
init_jfs(int size);
void
end_jfs(jfs_t* fs);

int
init_jarea(jarea_t* jarea, unsigned long max_block_size);
void
end_jarea(jarea_t* jarea);
int
jarea_write(jfs_t* fs, unsigned long lba, size_t n, unsigned long fid);
int
jarea_read(jfs_t* fs, unsigned long lba, size_t n, unsigned long fid);

int
jfs_write(jfs_t* fs, unsigned long lba, size_t n, unsigned long fid);
int
jfs_read(jfs_t* fs, unsigned long lba, size_t n, unsigned long fid);
int
jfs_delete(jfs_t* fs, unsigned long lba, size_t n, unsigned long fid);
void
jfs_check_out(jfs_t* jfs);
void
flush_command_table(transaction_head_t* head,
                    struct disk* d,
                    unsigned long offset);
void
flush_jarea(jarea_t* jarea);
