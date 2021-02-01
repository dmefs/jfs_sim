#include <stdio.h>
#include <stdlib.h>
#include "jfs.h"
#include "command_table.h"
#include "../IMRSimulator/src/lba.h"

jfs_t jfs;

jfs_operations jfs_ops {
    .read = jfs_read,
    .write = jfs_write,
};


jfs_t *init_jfs(int size)
{
    void *p = NULL;

    if (!(p = malloc(sizeof(struct disk))))
        goto done_create_disk;
    jfs.d = (struct disk *)p;
    if (init_disk(jfs.d, size)) {
        fprintf(stderr, "Error: Failed to init_disk.\n");
        exit(EXIT_FAILURE);
    }

    jfs.jarea.max_jarea_num = jfs.d->max_block_num / 10;
    if (init_command_table(&jfs.head, jfs.jarea.max_jarea_num))
        goto done_command_table;

    jfs.jfs_op = &jfs_ops;

    return &jfs;
done_command_table:
    free(jfs.d);
done_create_disk:
    return NULL;
}

void end_jfs(jfs_t *fs)
{
    end_disk(fs->d);
    end_command_table(&fs->head);
}

int jarea_write(jfs_t *fs, unsigned long lba, size_t n)
{
    fs->jarea.jarea_size += BYTE_TO_BLOCK(n);
    return fs->d->d_op->write(fs->d, lba, n);
}

int jarea_read(jfs_t *fs, unsigned long lba, size_t n) 
{
    size_t num = BYTE_TO_BLOCK(n);
    if ((lba + num) > fs->jarea.max_jarea_num) {
        fprintf(stderr, "Error: Failed to Read jarea. Out of boundary.\n");
        return 0;
    }
    return fs->d->d_op->read(fs->d, lba, n);
}

bool jarea_is_full(jarea_t *jarea, unsigned long lba, size_t n)
{
    return (jarea->jarea_size + n) > jarea->max_jarea_num;
}

void jfs_check_out(jfs_t *fs)
{
    unsigned long offset = fs->jarea.max_jarea_num;
    transaction_head_t *head = &fs->head;
    for (size_t i = 0; i < head->size; i++) {
        transaction_t *t = &head->table[i];
        fs->d->d_op->write(fs->d, t->lba + offset, t->size);
    }
    head->size = 0;
}

int jfs_write(jfs_t *fs, unsigned long lba, size_t n)
{
    if (jarea_is_full(&fs->jarea, lba, n))
        jfs_check_out(fs);
    unsigned long jarea_lba = fs->jarea.jarea_size;
    add_command_table(&fs->head, lba, n, jarea_lba);
    return jarea_write(fs, jarea_lba, n);
}

int jfs_read(jfs_t *fs, unsigned long lba, size_t n)
{
    unsigned long jarea_lba = 0;
    if (in_command_table(&fs->head, lba, n, &jarea_lba))
        return jarea_read(fs, jarea_lba, n);
    else
        return lba_read(fs->d, lba, n);
}
