#include "containers-internal.h"
#include <stdlib.h>

/* declarations */
static
LIBMISC_BOOL_TYPE realloc_shrinkable(buffer_t *b, size_t newsize);
static
LIBMISC_BOOL_TYPE realloc_nonshrinkable(buffer_t *b, size_t newsize);
static
LIBMISC_BOOL_TYPE realloc_economic(buffer_t *b, size_t newsize);

static
list_element_t *alloc_lel_inplace(list_t *l);
static
list_element_t *alloc_lel_simple(list_t *l);

/* data */
const buffer_reallocer_func buffer_reallocer[buffer_policy_max] = {
    [bp_shrinkable]      = realloc_shrinkable,
    [bp_non_shrinkable]  = realloc_nonshrinkable,
    [bp_economic]        = realloc_economic
};

const struct LIST_ALLOCATION_FUNCS list_el_alloc_dealloc[2] = {
    [LIBMISC_TRUE]  = {
        .allocator      = alloc_lel_inplace,
        .deallocator    = (list_el_deallocator)free
    },
    [LIBMISC_FALSE] = {
        .allocator      = alloc_lel_simple,
        .deallocator    = (list_el_deallocator)free
    }
};

/* definitions */
LIBMISC_BOOL_TYPE realloc_shrinkable(buffer_t *b, size_t newsize) {
    void *d;
    assert(b);
    d = realloc(b->data, newsize);

    if (d) {
        b->user_size = b->real_size = newsize;
        b->data = d;
    }

    return LIBMISC_AS_BOOL(d);
}

LIBMISC_BOOL_TYPE realloc_nonshrinkable(buffer_t *b, size_t newsize) {
    void *d;

    assert(b);

    if (newsize < b->real_size) {
        b->user_size = newsize;
        return LIBMISC_TRUE;
    }

    d = realloc(b->data, newsize);

    if (d) {
        b->user_size = newsize;
        b->real_size = newsize;
        b->data = d;
    }

    return LIBMISC_AS_BOOL(d);
}

LIBMISC_BOOL_TYPE realloc_economic(buffer_t *b, size_t newsize) {
    assert(b);

    size_t lesser = b->real_size / 2;
    size_t greater = b->real_size * 2;
    size_t rs;
    void *d;

    if (newsize < b->real_size / 4)
        rs = lesser;
    else if (newsize > b->real_size) {
        if (newsize < greater)
            rs = greater;
        else
            rs = newsize;
    }
    else {
        b->user_size = newsize;
        return LIBMISC_TRUE;
    }

    d = realloc(b->data, rs);

    if (d) {
        b->user_size = newsize;
        b->real_size = rs;
        b->data = d;
    }

    return LIBMISC_AS_BOOL(d);
}

list_element_t *alloc_lel_inplace(list_t *l) {
    list_element_t *le = malloc(sizeof(list_element_t) + l->element_size);

    assert(le);

    le->data = le + 1;
    le->host = l;
    le->next = le->prev = NULL;
    return le;
}

list_element_t *alloc_lel_simple(list_t *l) {
    list_element_t *le = malloc(sizeof(list_element_t));

    assert(le);

    le->data = NULL;
    le->host = l;
    le->next = le->prev = NULL;
    return le;
}
