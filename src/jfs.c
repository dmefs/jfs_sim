#include "jfs.h"
#include <stdio.h>
#include <stdlib.h>
#include "command_table.h"
#include "lba.h"
#include "op_mode.h"

jfs_t jfs;
op_mode_t recording_mode;

jfs_operations jfs_ops = {
    .read = jfs_read,
    .write = jfs_write,
    .delete = jfs_delete,
};

jfs_t *init_jfs(int size)
{
    void *p = NULL;

    if (!(p = malloc(sizeof(struct disk))))
        goto done_create_disk;
    jfs.d = (struct disk *) p;
    if (init_disk(jfs.d, size)) {
        fprintf(stderr, "Error: Failed to init_disk.\n");
        exit(EXIT_FAILURE);
    }

    if (init_jarea(&jfs.jarea, jfs.d->report.max_block_num))
        goto done_jarea;
    if (init_command_table(&jfs.head, jfs.jarea.max_jarea_num))
        goto done_command_table;
    if (init_reference_table(&reference_table, jfs.d->report.max_block_num))
        goto done_reference_table;
    jfs.jfs_op = &jfs_ops;

    return &jfs;

done_reference_table:
    end_command_table(&jfs.head);
done_command_table:
    end_disk(jfs.d);
done_jarea:
done_create_disk:
    return NULL;
}

void end_jfs(jfs_t *fs)
{
    end_reference_table(reference_table);
    end_command_table(&fs->head);
    end_jarea(&jfs.jarea);
    end_disk(fs->d);
}

int init_jarea(jarea_t *jarea, unsigned long max_block_size)
{
    unsigned long jarea_block_size = max_block_size / 100;
    jarea->max_jarea_num = jarea_block_size;
    jarea->capacity = jarea_block_size;
    jarea->size = 0;
    return 0;
}

void end_jarea(jarea_t *jarea)
{
    jarea->max_jarea_num = 0;
    jarea->capacity = 0;
    jarea->size = 0;
}

int jarea_write(jfs_t *fs, unsigned long lba, size_t n, unsigned long fid)
{
    recording_mode = journaling_op_mode;
    fs->jarea.size += n;
    return fs->d->d_op->journaling_write(fs->d, lba, n, fid);
}

int jarea_read(jfs_t *fs, unsigned long lba, size_t n, unsigned long fid)
{
    recording_mode = journaling_op_mode;
    if ((lba + n) > fs->jarea.max_jarea_num) {
        fprintf(
            stderr,
            "Error: Failed to Read jarea. Out of boundary, lba: %lu, n: %lu\n",
            lba, n);
        return 0;
    }
    return fs->d->d_op->read(fs->d, lba, n, fid);
}

bool jarea_is_full(jarea_t *jarea, size_t n)
{
    return ((jarea->size >= jarea->capacity) ||
            ((jarea->size + n) >= jarea->capacity));
}

void jfs_check_out(jfs_t *jfs)
{
    flush_command_table(&jfs->head, jfs->d, jfs->jarea.max_jarea_num);
    flush_jarea(&jfs->jarea);
}

int jfs_write(jfs_t *fs, unsigned long lba, size_t n, unsigned long fid)
{
    if ((lba + n + fs->jarea.max_jarea_num) > fs->d->report.max_block_num)
        return 0;
    if (jarea_is_full(&fs->jarea, n))
        jfs_check_out(fs);
    unsigned long jarea_lba = fs->jarea.size;
    add_command_table(&fs->head, lba, n, jarea_lba, fid);
    return jarea_write(fs, jarea_lba, n, fid);
}

int jfs_read(jfs_t *fs, unsigned long lba, size_t n, unsigned long fid)
{
    recording_mode = normal_op_mode;
    unsigned long jarea_lba = 0;
    if ((lba + n + fs->jarea.max_jarea_num) > fs->d->report.max_block_num)
        return 0;
    if (in_command_table(&fs->head, lba, n, &jarea_lba, fid))
        return jarea_read(fs, jarea_lba, n, fid);
    else
        return lba_read(fs->d, lba + fs->jarea.max_jarea_num, n, fid);
}

void flush_command_table(transaction_head_t *head,
                         struct disk *d,
                         unsigned long offset)
{
    for (size_t i = 0; i < head->size; i++) {
        transaction_t *t = &head->table[i];
        if (t->valid) {
            recording_mode = journaling_op_mode;
            d->d_op->read(d, t->jarea_lba, t->size, t->fid);
            recording_mode = normal_op_mode;
            d->d_op->write(d, t->lba + offset, t->size, t->fid);
            recording_mode = journaling_op_mode;
            d->d_op->invalid(d, t->jarea_lba, t->size, t->fid);
            invalid_reference_table(reference_table, t->lba);
        }
    }
    head->size = 0;
}

void delete_fid_command_table(transaction_head_t *head, unsigned long fid)
{
    for (size_t i = 0; i < head->size; i++) {
        transaction_t *t = &head->table[i];
        if (t->fid == fid) {
            t->valid = false;
            for (size_t j = 0; j < t->size; j++)
                invalid_reference_table(reference_table, t->lba + j);
        }
    }
}

int jfs_delete(jfs_t *fs, unsigned long lba, size_t n, unsigned long fid)
{
    delete_fid_command_table(&fs->head, fid);
    return fs->d->d_op->remove(fs->d, lba, n, fid);
}

void flush_jarea(jarea_t *jarea)
{
    jarea->size = 0;
}
