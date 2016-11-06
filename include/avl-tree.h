#ifndef _AVL_TREE_H_
# define _AVL_TREE_H_

/** \file avl-tree.h
 * AVL tree library
 */

# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>

struct avl_tree;
typedef struct avl_tree avl_tree_t;

struct avl_tree_node;
typedef struct avl_tree_node avl_tree_node_t;

typedef int64_t avl_tree_key_t;

struct avl_tree_node {
    /*! host tree */
    avl_tree_t *host;
    /*! less-than node */
    avl_tree_node_t *left;
    /*! more-than node */
    avl_tree_node_t *right;
    /*! parent node */
    avl_tree_node_t *parent;
    unsigned char height;
    avl_tree_key_t key;
    void *data;
};

struct avl_tree {
    size_t count;
    bool inplace;
    size_t node_data_size;
    avl_tree_node_t *root;
};

/**
 * Initialize tree at \c tree.
 * Tree node payload may be allocated inplace according to \c inplace value.
 * Node payload size is set with \c node_data_size.
 */
void avl_tree_init(avl_tree_t *tree, bool inplace, size_t node_data_size);
/**
 * Fetch tree node with key \c k
 */
avl_tree_node_t *avl_tree_get(avl_tree_t *t, avl_tree_key_t k);
/**
 * Add another tree node with key \c k
 */
avl_tree_node_t *avl_tree_add(avl_tree_t *t, avl_tree_key_t k);
/**
 * Either add or fetch (if already exists) tree node with key \c k.
 * Value in \c *_inserted will be set to \c true if node was inserted.
 * Otherwise the value in \c *_inserted will be unchanged.
 */
avl_tree_node_t *avl_tree_add_or_get(avl_tree_t *t, avl_tree_key_t k,
                                     bool *_inserted);
/**
 * Remove tree node with key \c k and fetch it's payload pointer payload pointer
 * \return payload pointer or \c NULL if there is no tree node with key
 *         value provided
 * Use of this function may succeed to segfault if tree node's payload is
 * allocated inplace with the node.
 */
void *avl_tree_remove_get_data(avl_tree_t *t, avl_tree_key_t k);
/**
 * Remove tree node with key \c k and fetch it's payload pointer payload pointer
 * \return payload pointer or \c NULL if there is no tree node with key
 *         value provided
 * Value in \c *removed will indicate if there was tree node with key value \c k.
 */
void *avl_tree_remove_get_data_signal(avl_tree_t *t, avl_tree_key_t k,
                                      bool *removed);
/**
 * Remove tree node with key \c k
 */
void avl_tree_remove(avl_tree_t *t, avl_tree_key_t k);
/* void avl_tree_remove_direct(avl_tree_t *t, avl_tree_node_t *n); */
/**
 * Fetch next tree node in ascending order of key
 * \return tree node pointer or \c NULL if there is no node with larger key
 */
avl_tree_node_t *avl_tree_node_next(avl_tree_node_t *n);
/**
 * Fetch next tree node in descending order of key
 * \return tree node pointer or \c NULL if there is no node with smaller key
 */
avl_tree_node_t *avl_tree_node_prev(avl_tree_node_t *n);
/**
 * Find minimum key node in subtree
 */
avl_tree_node_t *avl_tree_node_min(avl_tree_node_t *n);
/**
 * Find maximum key node in subtree
 */
avl_tree_node_t *avl_tree_node_max(avl_tree_node_t *n);
void avl_tree_purge(avl_tree_t *tree);

#endif