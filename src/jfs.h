#pragma once

#include "IMRSimulator/src/lba.h"

#define BYTE_TO_BLOCK(n) (((n) / SECTOR_SIZE) + !!((n) % SECTOR_SIZE))

typedef struct {
    transaction_t *table;
    bool (*is_full)(transaction_head_t *head);
    size_t capacity;
    size_t size;
} transaction_head_t;

typedef struct {
    unsigned long lba;
    unsigned long jarea_lba;
    size_t size;
} transaction_t;

typedef struct {
    size_t max_jarea_num;
    unsigned long jarea_size;
} jarea_t;

typedef struct {
    struct disk *d;
    transaction_head_t head;
    jarea_t jarea;
} jfs_t;

extern jfs_t jfs;
int init_jfs(int size);
