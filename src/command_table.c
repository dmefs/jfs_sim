#include "command_table.h"

ref_t* reference_table;

int
init_command_table(transaction_head_t* head, size_t num)
{
    void* p;
    head->capacity = num;
    if (!(p = calloc(num, sizeof(transaction_t)))) {
        fprintf(stderr, "Error: Failed to allocate jfs command table.\n");
        exit(EXIT_FAILURE);
    }
    head->table = (transaction_t*)p;
    head->size = 0;
    return 0;
}

void
end_command_table(transaction_head_t* head)
{
    free(head->table);
}

void
extend_table(transaction_head_t* head)
{
    transaction_t** t = &head->table;

    if (!(*t = (transaction_t*)realloc(*t, sizeof(**t) * 2 * head->capacity))) {
        fprintf(stderr, "Error: Failed to extend jfs command table.\n");
        exit(EXIT_FAILURE);
    }
    head->capacity *= 2;
}

void
add_command_table(transaction_head_t* head,
                  unsigned long lba,
                  size_t n,
                  unsigned long jarea_lba,
                  unsigned long fid)
{
    if (table_is_full(head)) {
        extend_table(head);
    }
    transaction_t* t = &head->table[head->size];
    t->lba = lba;
    t->jarea_lba = jarea_lba;
    t->size = n;
    t->fid = fid;
    t->valid = true;
    for (size_t i = 0; i < n; i++)
        add_reference_table(reference_table, lba + i, jarea_lba + i);
    head->size++;
}

bool
in_command_table(transaction_head_t* head,
                 unsigned long lba,
                 size_t n,
                 unsigned long* jarea_lba,
                 unsigned long fid)
{
    ref_t* r = &reference_table[lba];
    if (!r->valid)
        return false;
    transaction_t* t = &head->table[r->transaction_index];
    if (!t->valid)
        return false;
    if ((t->fid == fid) && (t->lba <= lba) &&
        ((t->lba + t->size) >= (lba + n))) {
        *jarea_lba = t->jarea_lba;
        return true;
    }
    return false;
}

int
init_reference_table(ref_t** p, size_t n)
{
    if ((*p = calloc(n, sizeof(*reference_table))) == NULL) {
        fprintf(stderr, "Error: fail to init reference table.\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

void
end_reference_table(ref_t* t)
{
    free(t);
}

void
add_reference_table(ref_t* t, unsigned long lba, unsigned long index)
{
    t[lba].transaction_index = index;
    t[lba].valid = true;
}

void
invalid_reference_table(ref_t* t, unsigned long lba)
{
    t[lba].valid = false;
}
