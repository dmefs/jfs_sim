#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    unsigned long lba;
    unsigned long jarea_lba;
    size_t size;
    int fid;
    bool valid;
} transaction_t;

typedef struct _transaction_head_t transaction_head_t;

struct _transaction_head_t {
    transaction_t *table;
    size_t capacity;
    size_t size;
};

struct _ref_t {
    unsigned long transaction_index;
    bool valid;
};

typedef struct _ref_t ref_t;

int init_command_table(transaction_head_t *head, size_t num);
void end_command_table(transaction_head_t *head);
void extend_table(transaction_head_t* head);
void add_command_table(transaction_head_t *head, unsigned long lba, size_t n, unsigned long jarea_lba, int fid);
bool in_command_table(transaction_head_t *head,unsigned long lba,size_t n,unsigned long *jarea_lba, int fid);
int init_reference_table(ref_t **p, size_t n);
void add_reference_table(ref_t *p, unsigned long lba, unsigned long index);
void invalid_reference_table(ref_t *p, unsigned long lba);
void end_reference_table(ref_t *t);

static inline bool table_is_full(transaction_head_t *head)
{
    return head->size == head->capacity;
}

extern ref_t *reference_table;