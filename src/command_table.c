#include "command_table.h"

int init_command_table(transaction_head_t *head, size_t num)
{
    void *p;
    head->capacity = num;
    if (!(p = malloc(sizeof(transaction_t) * num))) {
        fprintf(stderr, "Error: Failed to allocate jfs command table.\n");
        exit(EXIT_FAILURE);
    }
    head->table = (transaction_t*) p;
    head->size = 0;
    head->is_full = table_is_full;
    return 0;
}

void end_command_table(transaction_head_t *head)
{
    free(head->table);
}

inline bool table_is_full(transaction_head_t *head)
{
    return head->size == head->capacity;
}

void extend_table(transaction_head_t* head)
{
    transaction_t **t = &head->table;

    if (!(*t = (transaction_t *)realloc(*t, sizeof(**t) * 2 * head->capacity))) {
        fprintf(stderr, "Error: Failed to extend jfs command table.\n");
        exit(EXIT_FAILURE);
    }
    head->capacity *= 2;
}

void add_command_table(transaction_head_t *head, unsigned long lba, size_t n, unsigned long jarea_lba)
{
    if (table_is_full(head)) {
        extend_table(head);
    }
    transaction_t *t = &head->table[head->size];
    t->lba = lba;
    t->jarea_lba = jarea_lba;
    t->size = n;
    head->size++;
}

bool in_command_table(transaction_head_t *head,unsigned long lba,size_t n,unsigned long *jarea_lba)
{
    for (size_t i = head->size - 1; (i + 1) > 0; i--) {
        transaction_t *t = &head->table[i];
        if ((t->lba == lba) && (t->size = n)) {
            *jarea_lba = t->jarea_lba;
            return true;
        }
    }
    return false;
}