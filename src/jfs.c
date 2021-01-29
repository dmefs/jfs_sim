#include <stdio.h>
#include <stdlib.h>
#include "jfs.h"
#include "IMRSimulator/src/lba.h"

jfs_t jfs;

int init_command_table(jfs_t *fs, size_t num)
{
    void *p;
    transaction_head_t *head = &fs->head;
    fs->head.capacity = num;
    if (!(p = malloc(sizeof(transaction_t) * num))) {
        fprintf(stderr, "Error: Failed to allocate jfs command table.\n");
        exit(EXIT_FAILURE);
    }
    head->table = (transaction_t*) p;
    head->size = 0;
    head->is_full = table_is_full;
    return 0;
}

inline bool table_is_full(transaction_head_t *head)
{
    return head->size == head->capacity;
}

void extend_table(transaction_head_t* head)
{
    transaction_t **t = &head->table;

    if (!(*t = calloc(*t, sizeof(**t) * 2 * head->capacity))) {
        fprintf(stderr, "Error: Failed to extend jfs command table.\n");
        exit(EXIT_FAILURE);
    }
    head->capacity *= 2;
}

void add_command_table(transaction_head_t *head, unsigned long lba, size_t n, unsigned long jarea_lba)
{
    if (table_is_full(head)) {
        expand_table(head);
    }
    transaction_t *t = head->table[head->size];
    t->lba = lba;
    t->jarea_lba = jarea_lba;
    t->size = n;
    head->size++;
}

int init_jfs(int size)
{
    void *p = NULL;

    if (!(p = malloc(sizeof(struct disk))))
        goto done_create_disk;
    jfs.d = p;
    init_disk(jfs.d, size);

    jfs.jarea.max_jarea_num = jfs.d->max_block_num / 10;
    if (init_command_table(jfs.jarea.max_jarea_num))
        goto done_command_table;

    return 0;
done_command_table:
    free(jfs.d);
done_create_disk:
    return -1;
}

int jarea_write(jfs_t *fs, unsigned long lba, size_t n)
{
    fs->jarea.jarea_size += BYTE_TO_BLOCK(n);
    return lba_write(fs->d, lba, n);
}

int jarea_read(jfs_t *fs, unsigned long lba, size_t n) 
{
    size_t num = BYTE_TO_BLOCK(n);
    if ((lba + num) > fs->jarea.max_jarea_num) {
        fprintf(stderr, "Error: Failed to Read jarea. Out of boundary.\n");
        return 0;
    }
    return lba_read(fs->d, lba, n);
}

int jfs_write(jfs_t *fs, unsigned long lba, size_t n)
{
    if (jarea_is_full(fs, lba, n))
        jfs_check_out(fs);
    unsigned long jarea_lba = fs->jarea.jarea_size;
    add_command_table(fs, lba, n, jarea_lba);
    return jarea_write(fs, jarea_lba, n);
}

int jfs_read(jfs_t *fs, unsigned long lba, size_t n)
{
    unsigned long jarea_lba = 0;
    if (in_command_table(fs, lba, n, &jarea_lba))
        return jarea_read(fs->d, jarea_lba, n);
    else
        return lba_read(fs->d, lba, n);
}
