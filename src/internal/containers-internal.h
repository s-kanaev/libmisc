#ifndef CONTAINERS_INTERNAL_H
# define CONTAINERS_INTERNAL_H

# include "common.h"
# include "containers.h"

# include <stddef.h>
# include <stdint.h>

typedef LIBMISC_BOOL_TYPE (*buffer_reallocer_func)(buffer_t *b, size_t newsize);
typedef list_element_t *(*list_el_allocator)(list_t *l);
typedef void (*list_el_deallocator)(list_element_t *el);

struct LIST_ALLOCATION_FUNCS {
    list_el_allocator   allocator;
    list_el_deallocator deallocator;
};

extern const buffer_reallocer_func buffer_reallocer[buffer_policy_max];
extern const struct LIST_ALLOCATION_FUNCS list_el_alloc_dealloc[2];

#endif /* CONTAINERS_INTERNAL_H */
