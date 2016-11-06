#ifndef _CONTAINERS_H_
# define _CONTAINERS_H_

/** \file containers.h
 * Containers/collections library:
 * buffer, vector, array, list, queue, stack
 */

# include <stddef.h>
# include <stdbool.h>

enum buffer_policy {
    bp_shrinkable          = 0,
    bp_non_shrinkable      = 1,
    bp_economic            = 2,
    buffer_policy_max   = 3
};

/** Dumb buffer with shrinkability selection
 */
typedef struct buffer {
    enum buffer_policy pol;
    /** user data */
    void *data;
    /** size requested by user */
    size_t user_size;
    /** really allocated size */
    size_t real_size;
} buffer_t;

/** A vector with dynamic size
 */
typedef struct vector {
    buffer_t data;
    size_t element_size;
    size_t count;
} vector_t;

/** A doubly linked list
 */
typedef struct list list_t;
typedef struct list_element {
    list_t *host;
    struct list_element *prev;
    struct list_element *next;
    void *data;
} list_element_t;

struct list {
    list_element_t *front;
    list_element_t *back;
    size_t count;
    bool inplace;
    size_t element_size;
};

/**** buffer operations ****/
/**
 * Initialize buffer \c b with initial size \c size and policy \c pol
 */
void buffer_init(buffer_t *b,
                 size_t size,
                 enum buffer_policy pol);
/**
 * Change buffer \c b size to \c newsize according to buffer's policy
 */
bool buffer_realloc(buffer_t *b, size_t newsize);
/**
 * Deallocate allocated memory
 */
void buffer_deinit(buffer_t *b);

/**** vector operations ****/
/**
 * Initialize vector at \c v with initial number of elements \c count
 * and each element of size \c size
 */
void vector_init(vector_t *v, size_t size, size_t count);
/**
 * Deallocate vector
 */
void vector_deinit(vector_t *v);
/**
 * Remove single element from vector
 */
void vector_remove(vector_t *v, size_t idx);
/**
 * Remove consecutive elements from vector
 */
void vector_remove_range(vector_t *v, size_t from, size_t count);
/**
 * Insert element to vector \c v at index \c idx
 * \return pointer to inserted element
 */
void *vector_insert(vector_t *v, size_t idx);
/**
 * Insert element to vector \c v at the end of the vector
 * \return pointer to inserted element
 */
void *vector_append(vector_t *v);
/**
 * Insert element to vector \c v at the beginning of the vector
 * \return pointer to inserted element
 */
void *vector_prepend(vector_t *v);
/**
 * Fetch the first vector element address
 * \return pointer to the first element in vector
 */
void *vector_begin(vector_t *v);
/**
 * Fetch address of the element after the last one in vector
 * \return pointer to the after-the-last-one element of the vector
 */
void *vector_end(vector_t *v);
/**
 * Fetch vector element at index \c idx
 * \return pointer to the element
 */
void *vector_get(vector_t *v, size_t idx);
/**
 * Get next element pointer
 */
void *vector_next(vector_t *v, void *d);
/**
 * Get previous element pointer
 */
void *vector_prev(vector_t *v, void *d);

/**** list operations ****/
/**
 * Initialize list at \c l
 * List element payload may be allocated inplace with \c list_element_t
 * according to value of \c inplace.
 * List element payload size passed in \c size
 */
void list_init(list_t *l, bool inplace, size_t size);
/**
 * Return number of elements in list
 */
size_t list_size(const list_t *l);
/**
 * Add element to the list at the beginning
 */
list_element_t *list_prepend(list_t *l);
/**
 * Add element to the list at the end
 */
list_element_t *list_append(list_t *l);
/**
 * Add element to the list after the element.
 * The element is prepended when \c el equals nil
 */
list_element_t *list_add_after(list_t *l, list_element_t *el);
/**
 * Add element to the list right before the element
 * The element is appended when \c el equals nil
 */
list_element_t *list_add_before(list_t *l, list_element_t *el);
/**
 * Remove elemnt from the list and return pointer to the next one
 */
list_element_t *list_remove_and_advance(list_t *l, list_element_t *el);
/**
 * Deallocate the list
 */
void list_purge(list_t *l);
/**
 * Return the first list element pointer.
 * \return pointer or \c NULL if there are no elements in the list yet
 */
list_element_t *list_begin(list_t *l);
/**
 * Return the last list element pointer.
 * \return pointer or \c NULL if there are no elements in the list yet
 */
list_element_t *list_end(list_t *l);
/**
 * Return the next list element pointer.
 * \return pointer or \c NULL if there are no more elements
 */
list_element_t *list_next(list_t *l, list_element_t *el);
/**
 * Return the previous list element pointer.
 * \return pointer or \c NULL if there are no more elements
 */
list_element_t *list_prev(list_t *l, list_element_t *el);

#endif
