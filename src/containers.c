#include "containers.h"
#include "internal/containers-internal.h"
#include "common.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define LIBMISC_CONTAINERS_RETURN_WITH_ERROR(err, v)    \
do {                                                    \
    errno = (err);                                      \
    return v;                                           \
} while (0)

/***************************** BUFFER *****************************/
LIBMISC_INIT_RETURN_TYPE
buffer_init(buffer_t *b,
            size_t size,
            enum buffer_policy pol) {
    LIBMISC_MAKE_ASSERTION_OR_ACT(
        b && pol < buffer_policy_max,
        LIBMISC_INIT_RETURN_ERROR(EINVAL)
    );

    b->pol = pol;
    b->real_size = b->user_size = size;
    b->data = malloc(b->user_size);

    if (b->data)
        LIBMISC_INIT_RETURN_SUCCESS();
    else
        LIBMISC_INIT_RETURN_ERROR(ENOMEM);
}

LIBMISC_BOOL_TYPE buffer_realloc(buffer_t *b, size_t newsize) {
    LIBMISC_BOOL_TYPE rc;

    assert(b);

    rc = buffer_reallocer[b->pol](b, newsize);

    if (LIBMISC_BOOL_IS_FALSE(rc))
        errno = ENOMEM;

    return rc;
}

void buffer_deinit(buffer_t *b) {
    assert(b);

    if (!b)
        return;

    if (b->data)
        free(b->data);

    b->user_size = b->real_size = 0;
}

/***************************** VECTOR *****************************/
LIBMISC_BOOL_TYPE vector_init(vector_t *v,
                              size_t size,
                              size_t count) {
    LIBMISC_MAKE_ASSERTION_OR_ACT(
        v,
        LIBMISC_INIT_RETURN_ERROR(EINVAL)
    );

    v->element_size = size;
    v->count = count;
    return buffer_init(&v->data, size * count, bp_economic);
}

void vector_remove(vector_t *v, size_t idx) {
    size_t shifting;
    void *newpos;
    LIBMISC_BOOL_TYPE realloced;

    assert(v);
    assert(v->count > idx);

    -- v->count;

    newpos = v->data.data + idx * v->element_size;
    shifting = (v->count - idx) * v->element_size;
    memmove(
        newpos,
        newpos + v->element_size,
        shifting
    );

    realloced = buffer_realloc(&v->data, v->count * v->element_size);
    assert(realloced);
    LIBMISC_DONT_USE(realloced);
}

void vector_remove_range(vector_t *v, size_t from, size_t count) {
    size_t shifting;
    void *newpos;
    LIBMISC_BOOL_TYPE realloced;

    if (!count)
        return;

    assert(v);
    assert(v->count > from && v->count > from + count - 1);

    v->count -= count;
    newpos = v->data.data + from * v->element_size;
    shifting = (v->count - from) * v->element_size;

    memmove(
        newpos,
        newpos + count * v->element_size,
        shifting
    );

    realloced = buffer_realloc(&v->data, v->count * v->element_size);
    assert(realloced);
    LIBMISC_DONT_USE(realloced);
}

void *vector_insert(vector_t *v, size_t idx) {
    size_t shifting;
    void *newelement;
    void *newpos;
    LIBMISC_BOOL_TYPE realloced;

    LIBMISC_MAKE_ASSERTION_OR_ACT(
        v && (idx <= v->count),
        LIBMISC_CONTAINERS_RETURN_WITH_ERROR(EINVAL, NULL)
    );

    newelement = v->data.data + v->element_size * idx;
    newpos = newelement + v->element_size;
    shifting = v->element_size * (v->count - idx);

    ++v->count;

    realloced = buffer_realloc(&v->data, v->element_size * (v->count + 1));

    LIBMISC_MAKE_ASSERTION_OR_ACT(
        LIBMISC_BOOL_IS_TRUE(realloced),
        LIBMISC_CONTAINERS_RETURN_WITH_ERROR(ENOMEM, NULL)
    );

    memmove(newpos, newelement, shifting);

    return newelement;
}

void *vector_append(vector_t *v) {
    LIBMISC_MAKE_ASSERTION_OR_ACT(
        v,
        LIBMISC_CONTAINERS_RETURN_WITH_ERROR(EINVAL, NULL)
    );

    size_t filled = v->element_size * v->count;
    void *d;
    LIBMISC_BOOL_TYPE realloc_ret = buffer_realloc(&v->data, filled + v->element_size);

    LIBMISC_MAKE_ASSERTION_OR_ACT(
        LIBMISC_BOOL_IS_TRUE(realloc_ret),
        LIBMISC_CONTAINERS_RETURN_WITH_ERROR(ENOMEM, NULL)
    );

    d = v->data.data + filled;
    ++v->count;

    return d;
}

void *vector_prepend(vector_t *v) {
    LIBMISC_MAKE_ASSERTION_OR_ACT(
        v,
        LIBMISC_CONTAINERS_RETURN_WITH_ERROR(EINVAL, NULL)
    );

    size_t filled = v->element_size * v->count;
    void *d;
    LIBMISC_BOOL_TYPE realloc_ret = buffer_realloc(&v->data, filled + v->element_size);

    LIBMISC_MAKE_ASSERTION_OR_ACT(
        LIBMISC_BOOL_IS_TRUE(realloc_ret),
        LIBMISC_CONTAINERS_RETURN_WITH_ERROR(ENOMEM, NULL)
    );

    memmove(
        v->data.data + v->element_size,
        v->data.data,
        filled
    );

    d = v->data.data;
    ++v->count;

    return d;
}

void *vector_begin(vector_t *v) {
    assert(v);
    return v->data.data;
}

void *vector_end(vector_t *v) {
    assert(v);
    return v->data.data + v->count * v->element_size;
}

void *vector_get(vector_t *v, size_t idx) {
    assert(v);
    assert(v->count > idx);

    return v->data.data + idx * v->element_size;
}

void *vector_next(vector_t *v, void *d) {
    assert(v);
    assert(!(d < v->data.data));
    assert(d < v->data.data + v->count * v->element_size);
    assert((d - v->data.data) % v->element_size == 0);

    return d + v->element_size;
}

void *vector_prev(vector_t *v, void *d) {
    assert(v);
    assert(!(d < v->data.data));
    assert(d < v->data.data + v->count * v->element_size);
    assert((d - v->data.data) % v->element_size == 0);

    return d == v->data.data ? d : d - v->element_size;
}

void vector_deinit(vector_t *v) {
    assert(v);
    buffer_deinit(&v->data);
    v->count = 0;
    v->element_size = 0;
}

/***************************** LIST *****************************/
LIBMISC_BOOL_TYPE list_init(list_t *l,
                            LIBMISC_BOOL_TYPE inplace,
                            size_t size) {
    LIBMISC_MAKE_ASSERTION_OR_ACT(
        !inplace || (inplace && size),
        LIBMISC_INIT_RETURN_ERROR(EINVAL)
    );

    l->front = l->back = NULL;

    l->element_size = size;
    l->inplace = inplace;

    l->count = 0;

    LIBMISC_INIT_RETURN_SUCCESS();
}

size_t list_size(const list_t *l) {
    assert(l);
    return l->count;
}

list_element_t *list_prepend(list_t *l) {
    list_element_t *el;

    LIBMISC_MAKE_ASSERTION_OR_ACT(
        l,
        LIBMISC_CONTAINERS_RETURN_WITH_ERROR(EINVAL, NULL)
    );

    el = list_el_alloc_dealloc[l->inplace].allocator(l);

    LIBMISC_MAKE_ASSERTION_OR_ACT(
        el,
        LIBMISC_CONTAINERS_RETURN_WITH_ERROR(ENOMEM, NULL)
    );

    el->prev = NULL;
    el->next = l->front;
    l->front = el;

    if (el->next)
        el->next->prev = el;

    if (!l->back)
        l->back = el;

    ++ l->count;
    return el;
}

list_element_t *list_append(list_t *l) {
    list_element_t *el;

    LIBMISC_MAKE_ASSERTION_OR_ACT(
        l,
        LIBMISC_CONTAINERS_RETURN_WITH_ERROR(EINVAL, NULL)
    );

    el = list_el_alloc_dealloc[l->inplace].allocator(l);

    LIBMISC_MAKE_ASSERTION_OR_ACT(
        el,
        LIBMISC_CONTAINERS_RETURN_WITH_ERROR(ENOMEM, NULL)
    );

    el->prev = l->back;
    el->next = NULL;
    l->back = el;

    if (el->prev)
        el->prev->next = el;

    if (!l->front)
        l->front = el;

    ++ l->count;
    return el;
}

list_element_t *list_add_after(list_t *l, list_element_t *el) {
    list_element_t *newel;

    LIBMISC_MAKE_ASSERTION_OR_ACT(
        l && (!el || el->host == l),
        LIBMISC_CONTAINERS_RETURN_WITH_ERROR(EINVAL, NULL)
    );

    if (!el)
        return list_prepend(l);

    newel = list_el_alloc_dealloc[l->inplace].allocator(l);

    LIBMISC_MAKE_ASSERTION_OR_ACT(
        el,
        LIBMISC_CONTAINERS_RETURN_WITH_ERROR(ENOMEM, NULL)
    );

    newel->prev = el;

    if (newel->prev) {
        newel->next = el->next;

        newel->prev->next = newel;

        if (newel->next)
            newel->next->prev = newel;
    }

    if (!l->front)
        l->front = newel;

    if (!l->back || l->back == el)
        l->back = newel;

    ++ l->count;
    return newel;
}

list_element_t *list_add_before(list_t *l, list_element_t *el) {
    list_element_t *newel;

    LIBMISC_MAKE_ASSERTION_OR_ACT(
        l && (!el || el->host == l),
        LIBMISC_CONTAINERS_RETURN_WITH_ERROR(EINVAL, NULL)
    );

    if (!el)
        return list_append(l);

    LIBMISC_MAKE_ASSERTION_OR_ACT(
        el,
        LIBMISC_CONTAINERS_RETURN_WITH_ERROR(ENOMEM, NULL)
    );

    newel = list_el_alloc_dealloc[l->inplace].allocator(l);

    newel->next = el;

    if (newel->next) {
        newel->prev = el->prev;

        newel->next->prev = newel;

        if (newel->prev)
            newel->prev->next = newel;
    }

    if (!l->front || l->front == el)
        l->front = newel;

    if (!l->back)
        l->back = newel;

    ++ l->count;
    return newel;
}

list_element_t *list_remove_and_advance(list_t *l, list_element_t *el) {
    list_element_t *prev = NULL, *next = NULL;

    LIBMISC_MAKE_ASSERTION_OR_ACT(
        l && (!el || el->host == l),
        LIBMISC_CONTAINERS_RETURN_WITH_ERROR(EINVAL, NULL)
    );

    if (!l->count)
        return NULL;

    if (el) {
        prev = el->prev;
        next = el->next;
        list_el_alloc_dealloc[l->inplace].deallocator(el);
    }

    if (prev)
        prev->next = next;

    if (next)
        next->prev = prev;

    -- l->count;

    if (el == l->front)
        l->front = next;

    if (el == l->back)
        l->back = prev;

    return next;
}

void list_purge(list_t *l) {
    list_element_t *el, *el2;

    assert(l);

    if (!l->count) {
        l->front = NULL;
        l->back = NULL;
        return;
    }

    for (el = l->front; el; el = el2) {
        el2 = el->next;
        list_el_alloc_dealloc[l->inplace].deallocator(el);
    }

    l->front = NULL;
    l->back = NULL;

    l->count = 0;
}

list_element_t *list_begin(list_t *l) {
    assert(l);
    return l->front;
}

list_element_t *list_end(list_t *l) {
    assert(l);
    return l->back;
}

list_element_t *list_next(list_t *l, list_element_t *el) {
    list_element_t *next;
    assert(l);
    assert(!el || el->host == l);

    next = (!el
            ? l->front : el->next);
    return next;
}

list_element_t *list_prev(list_t *l, list_element_t *el) {
    list_element_t *prev;
    assert(l);
    assert(!el || el->host == l);

    prev = (!el
            ? l->back : el->prev);
    return prev;
}
