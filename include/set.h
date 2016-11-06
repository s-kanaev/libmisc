#ifndef _SET_H_
# define _SET_H_

# include "avl-tree.h"

# include <stdbool.h>
# include <stddef.h>

typedef struct set {
    avl_tree_t tree;
} set_t;

typedef avl_tree_key_t set_key_t;

typedef unsigned int set_counter_t;

typedef struct set_iterator {
    avl_tree_node_t *it;
    set_counter_t count;
    set_key_t k;
} set_iterator_t;

void set_init(set_t *s);
void set_purge(set_t *s);

size_t set_size(set_t *s);
set_counter_t set_add(set_t *s, set_key_t k);
set_counter_t set_add_single(set_t *s, set_key_t k);
set_counter_t set_count(set_t *s, set_key_t k);
set_counter_t set_remove(set_t *s, set_key_t k);

set_iterator_t set_begin(set_t *s);
set_iterator_t set_end(set_t *s);
set_iterator_t set_next(set_t *s, avl_tree_node_t *el);
set_iterator_t set_prev(set_t *s, avl_tree_node_t *el);

set_key_t set_get_data(avl_tree_node_t *el);
set_counter_t set_data_count(avl_tree_node_t *el);

#endif /* _SET_H_ */