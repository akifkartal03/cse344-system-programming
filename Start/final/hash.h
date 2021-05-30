#ifndef _linear_hash_h
#define _linear_hash_h

typedef int element_type2;

typedef unsigned int pos;

typedef pos (*hash)(element_type2 e);

/* if equals, return 1, othewise, return 0 */
typedef int (*cmp_func)(element_type2 e, element_type2 b);

typedef enum kind_of_entry
{
    legitimate,
    empty,
    deleted
}kind_of_entry;

typedef struct hash_entry{
    element_type2 element;
    kind_of_entry info;
}cell;

typedef struct hash_table {
    hash hfunc;
    cmp_func equals;
    unsigned int table_size;
    cell *cells;
}*hash_table_ptr;

hash_table_ptr hash_init_table(unsigned int size, hash f, cmp_func c);

void hash_insert(hash_table_ptr hash_tbl, element_type2 e);

pos hash_find(hash_table_ptr hash_tbl, element_type2 e);

void hash_delete(hash_table_ptr hash_tbl, element_type2 e);

void hash_destroy_table(hash_table_ptr* ptr);

#endif