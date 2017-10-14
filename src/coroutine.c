#include "coroutine.h"
#include "common.h"

#include <assert.h>

static
void _caller(uint32_t dw1, uint32_t dw2) {
    uint64_t qw = dw2;
    coroutine_t *cr;

    qw <<= 0x20;
    qw |= dw1;
    cr = (coroutine_t *)qw;

    assert(cr);

    if (cr->cb)
        cr->cb(cr, cr->ctx);

    cr->returned = LIBMISC_TRUE;
}

LIBMISC_INIT_RETURN_TYPE
coroutine_init(coroutine_t *cr,
               coroutine_cb_t cb,
               void *ctx,
               size_t stack_size) {
    int rc;
    uint64_t qw;
    uint32_t dw1, dw2;

    LIBMISC_MAKE_ASSERTION_OR_ACT(
        cr,
        LIBMISC_INIT_RETURN_ERROR(EINVAL)
    );

    rc = getcontext(&cr->callee);

    LIBMISC_MAKE_ASSERTION_OR_ACT(
        0 == rc,
        LIBMISC_INIT_RETURN_ERROR(ENOSYS)
    );

    LIBMISC_INIT_RETURN_TYPE buffer_rc = buffer_init(
        &cr->stack,
        stack_size,
        bp_economic
    );

    LIBMISC_MAKE_ASSERTION_OR_ACT(
        LIBMISC_INIT_RETURN_IS_SUCCESS(buffer_rc),
        LIBMISC_INIT_RETURN_SAME_ERROR()
    );

    cr->cb = cb;
    cr->ctx = ctx;
    cr->returned = LIBMISC_FALSE;

    cr->callee.uc_link = &cr->caller;
    cr->callee.uc_stack.ss_size = stack_size;
    cr->callee.uc_stack.ss_sp = cr->stack.data;
    cr->callee.uc_stack.ss_flags = SS_ONSTACK;
    cr->callee.uc_flags = 0;

    qw = (uint64_t)cr;
    dw1 = qw & 0xffffffff;
    dw2 = (qw >> 0x20) & 0xffffffff;

    makecontext(&cr->callee, (void (*)())_caller, 2, dw1, dw2);

    LIBMISC_INIT_RETURN_SUCCESS();
}

void coroutine_deinit(coroutine_t *cr) {
    assert(cr);

    buffer_deinit(&cr->stack);
}

void coroutine_continue(coroutine_t *cr) {
    assert(cr);

    if (cr->returned)
        return;

    swapcontext(&cr->caller, &cr->callee);
}

void coroutine_yield(coroutine_t *cr) {
    assert(cr);

    swapcontext(&cr->callee, &cr->caller);
}

LIBMISC_BOOL_TYPE coroutine_returned(const coroutine_t *cr) {
    assert(cr);

    return cr->returned;
}
