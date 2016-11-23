#ifndef _COROUTINE_H_
# define _COROUTINE_H_

# include "containers.h"

# include <ucontext.h>
# include <stddef.h>
# include <stdint.h>

# ifdef __cplusplus
extern "C" {
# endif

struct coroutine;
typedef struct coroutine coroutine_t;

typedef void (*coroutine_cb_t)(coroutine_t *cr, void *ctx);

struct coroutine {
    buffer_t stack;

    ucontext_t caller;
    ucontext_t callee;

    coroutine_cb_t cb;
    void *ctx;

    bool returned;
};

void coroutine_init(coroutine_t *cr,
                    coroutine_cb_t cb, void *ctx,
                    size_t stack_size);
void coroutine_deinit(coroutine_t *cr);
void coroutine_continue(coroutine_t *cr);
bool coroutine_returned(const coroutine_t *cr);
void coroutine_yield(coroutine_t *cr);

# ifdef __cplusplus
}
# endif

#endif /* _COROUTINE_H_ */
