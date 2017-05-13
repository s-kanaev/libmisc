#include "io-service.h"
#include "avl-tree.h"
#include "containers.h"
#include "common.h"

#include <sys/eventfd.h>
#include <sys/epoll.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <assert.h>

static const int OP_MAP[IO_SVC_OP_COUNT] = {
    [IO_SVC_OP_READ] = EPOLLIN | EPOLLRDHUP | EPOLLPRI,
    [IO_SVC_OP_WRITE] = EPOLLOUT
};

/******************************* internal funcs *******************************/
void _notify(const io_service_t *iosvc) {
    static const uint64_t v = 1;
    ssize_t ret = write(iosvc->event_fd, &v, sizeof(v));

    assert(sizeof(v) == ret);
    DONT_USE(ret);
}

uint64_t _notfied(const io_service_t *iosvc) {
    uint64_t v;

    ssize_t ret = read(iosvc->event_fd, &v, sizeof(v));

    assert(sizeof(v) == ret);
    DONT_USE(ret);

    return v;
}

void _run_delayed_jobs(int fd, enum io_service_operation op,
                       io_service_t *iosvc, void *_ctx) {
    uint64_t stub UNUSED;
    list_element_t *el;
    iosvc_enqueued_op_t *enqued_op;
    iosvc_enqueued_op_cb_t cb;
    void *ctx;
    bool has_fresh = false;

    assert(fd == iosvc->event_fd);
    assert(op == IO_SVC_OP_READ);

    stub = _notfied(iosvc);

    pthread_mutex_lock(&iosvc->mtx);

    iosvc->running_enqueued = true;

    for (el = list_begin(&iosvc->enqueued_ops); el;) {
        enqued_op = el->data;

        if (enqued_op->fresh) {
            enqued_op->fresh = false;
            el = list_next(&iosvc->enqueued_ops, el);
            has_fresh = true;
            continue;
        }

        cb = enqued_op->cb;
        ctx = enqued_op->ctx;

        el = list_remove_and_advance(&iosvc->enqueued_ops, el);

        pthread_mutex_unlock(&iosvc->mtx);

        if (cb)
            cb(iosvc, ctx);

        pthread_mutex_lock(&iosvc->mtx);
    }

    iosvc->running_enqueued = false;
    pthread_mutex_unlock(&iosvc->mtx);

    if (has_fresh)
        _notify(iosvc);
}

void _run_masked(io_service_t *iosvc,
                 iosvc_fd_desc_t *fd_desc,
                 struct epoll_event *event) {
    int mask = 0;
    iosvc_fd_masked_op_t cb;
    void *ctx;
    bool oneshot;
    int fd;
    int rc;

    if (event->events & OP_MAP[IO_SVC_OP_READ])
        mask |= IO_SVC_OP_READ_MASK;
    if (event->events & OP_MAP[IO_SVC_OP_WRITE])
        mask |= IO_SVC_OP_WRITE_MASK;

    cb = fd_desc->op[IO_SVC_OP_COUNT].cb.masked_op;
    ctx = fd_desc->op[IO_SVC_OP_COUNT].ctx;
    oneshot = fd_desc->op[IO_SVC_OP_COUNT].oneshot;
    fd = fd_desc->fd;

    if (oneshot) {
        rc = epoll_ctl(iosvc->epoll_fd, EPOLL_CTL_DEL, fd, &fd_desc->event);
        assert(0 == rc);
        DONT_USE(rc);

        avl_tree_remove(&iosvc->fd_map, fd_desc->fd);
    }

    if (cb) {
        pthread_mutex_unlock(&iosvc->mtx);
        cb(fd, mask, iosvc, ctx);
        pthread_mutex_lock(&iosvc->mtx);
    }
}

void _run_unmasked(io_service_t *iosvc,
                   iosvc_fd_desc_t *fd_desc,
                   struct epoll_event *event) {
    iosvc_fd_op_t cb;
    void *ctx;
    bool oneshot;
    int fd, rc;
    int op;

    for (op = 0; op < IO_SVC_OP_COUNT; ++op) {
        if (!(event->events & OP_MAP[op]))
            continue;

        fd = fd_desc->fd;
        cb = fd_desc->op[op].cb.op;
        ctx = fd_desc->op[op].ctx;
        oneshot = fd_desc->op[op].oneshot;

        if (oneshot) {
            fd_desc->event.events &= ~OP_MAP[op];

            if (!fd_desc->event.events) {
                rc = epoll_ctl(iosvc->epoll_fd, EPOLL_CTL_DEL, fd, &fd_desc->event);
                assert(rc);
                DONT_USE(rc);

                avl_tree_remove(&iosvc->fd_map, fd);
            }
            else {
                memset(&fd_desc->op[op], 0, sizeof(fd_desc->op[op]));

                rc = epoll_ctl(iosvc->epoll_fd, EPOLL_CTL_MOD, fd, &fd_desc->event);
                assert(rc);
            }
        }

        if (cb) {
            pthread_mutex_unlock(&iosvc->mtx);
            cb(fd, op, iosvc, ctx);
            pthread_mutex_lock(&iosvc->mtx);
        }
    }
}

void _run_events(io_service_t *iosvc,
                 struct epoll_event *events, int events_number) {
    int idx;
    iosvc_fd_desc_t *fd_desc;

    for (idx = 0; idx < events_number; ++idx) {
        fd_desc = events[idx].data.ptr;

        pthread_mutex_lock(&iosvc->mtx);
        if (fd_desc->masked)
            _run_masked(iosvc, fd_desc, &events[idx]);
        else
            _run_unmasked(iosvc, fd_desc, &events[idx]);
        pthread_mutex_unlock(&iosvc->mtx);
    }
}

/******************************* API *******************************/
void io_service_init(io_service_t *iosvc) {
    int rc;

    assert(iosvc);

    rc = pthread_mutex_init(&iosvc->mtx, NULL);

    assert(rc);
    DONT_USE(rc);

    iosvc->event_fd = eventfd(0, EFD_CLOEXEC);
    assert(iosvc->event_fd >= 0);

    iosvc->epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    assert(iosvc->epoll_fd >= 0);

    iosvc->running = false;
    iosvc->allow_new_jobs = true;

    avl_tree_init(&iosvc->fd_map, true, sizeof(iosvc_fd_desc_t));
    list_init(&iosvc->enqueued_ops, true, sizeof(iosvc_enqueued_op_t));

    /* for enqueued functions processing */
    io_service_watch_fd(iosvc, iosvc->event_fd, IO_SVC_OP_READ,
                        _run_delayed_jobs, iosvc, false);
}

void io_service_deinit(io_service_t *iosvc) {
    int rc;

    assert(iosvc);

    list_purge(&iosvc->enqueued_ops);
    avl_tree_purge(&iosvc->fd_map);

    close(iosvc->epoll_fd);
    close(iosvc->event_fd);

    rc = pthread_mutex_destroy(&iosvc->mtx);
    assert(rc);
    DONT_USE(rc);
}

void io_service_enqueue_function(io_service_t *iosvc,
                                 iosvc_enqueued_op_cb_t f,
                                 void *ctx) {
    list_element_t *el;
    iosvc_enqueued_op_t *op;

    assert(iosvc);

    pthread_mutex_lock(&iosvc->mtx);

    el = list_append(&iosvc->enqueued_ops);

    op = el->data;
    op->cb = f;
    op->ctx = ctx;
    op->fresh = iosvc->running_enqueued;

    _notify(iosvc);

    pthread_mutex_unlock(&iosvc->mtx);
}

void io_service_watch_fd(io_service_t *iosvc,
                         int fd, enum io_service_operation op,
                         iosvc_fd_op_t f, void *ctx, bool oneshot) {
    iosvc_fd_desc_t *fd_desc;
    avl_tree_node_t *atn_fd;
    bool inserted = false;
    bool already_watched = true;
    int rc;
    int epoll_ctl_op;

    assert(iosvc);
    assert(op <= IO_SVC_OP_MAX);

    if (fd < 0)
        return;

    pthread_mutex_lock(&iosvc->mtx);

    if (!iosvc->allow_new_jobs) {
        pthread_mutex_unlock(&iosvc->mtx);
        return;
    }

    atn_fd = avl_tree_add_or_get(&iosvc->fd_map, fd, &inserted);

    assert(atn_fd && atn_fd->data);

    fd_desc = atn_fd->data;

    if (inserted) {
        fd_desc->fd = fd;
        fd_desc->masked = false;

        memset(fd_desc->op, 0, sizeof(fd_desc->op));

        memset(&fd_desc->event, 0, sizeof(fd_desc->event));

        fd_desc->event.data.ptr = fd_desc;

        already_watched = false;
    }

    assert(!fd_desc->masked);

    fd_desc->op[op].cb.op = f;
    fd_desc->op[op].ctx = ctx;
    fd_desc->op[op].oneshot = oneshot;

    fd_desc->event.events |= OP_MAP[op];

    if (already_watched)
        epoll_ctl_op = EPOLL_CTL_MOD;
    else
        epoll_ctl_op = EPOLL_CTL_ADD;

    rc = epoll_ctl(iosvc->epoll_fd, epoll_ctl_op,
                   fd_desc->fd, &fd_desc->event);

    assert(0 == rc);
    DONT_USE(rc);

    pthread_mutex_unlock(&iosvc->mtx);
}

void io_service_unwatch_fd(io_service_t *iosvc,
                           int fd, enum io_service_operation op) {
    iosvc_fd_desc_t *fd_desc;
    avl_tree_node_t *atn_fd;
    int rc;
    int epoll_ctl_op;
    void *ptr;

    assert(iosvc);
    assert(op <= IO_SVC_OP_MAX);

    if (fd < 0)
        return;

    pthread_mutex_lock(&iosvc->mtx);

    atn_fd = avl_tree_get(&iosvc->fd_map, fd);

    if (!atn_fd) {
        pthread_mutex_unlock(&iosvc->mtx);
        return;
    }

    fd_desc = atn_fd->data;

    assert(!fd_desc->masked);

    fd_desc->op[op].cb.op = NULL;
    fd_desc->op[op].ctx = NULL;
    fd_desc->event.events &= ~(OP_MAP[op]);

    if (fd_desc->event.events) {
        epoll_ctl_op = EPOLL_CTL_MOD;
        ptr = &fd_desc->event;
    }
    else {
        epoll_ctl_op = EPOLL_CTL_DEL;
        ptr = NULL;

        avl_tree_remove(&iosvc->fd_map, fd);
    }

    rc = epoll_ctl(iosvc->epoll_fd, epoll_ctl_op,
                   fd, ptr);

    assert(0 == rc);
    DONT_USE(rc);

    pthread_mutex_unlock(&iosvc->mtx);
}

void io_service_watch_fd_masked(io_service_t *iosvc,
                                int fd, int mask,
                                iosvc_fd_masked_op_t f, void *ctx,
                                bool oneshot) {
    iosvc_fd_desc_t *fd_desc;
    avl_tree_node_t *atn_fd;
    bool inserted = false;
    bool already_watched = true;
    int rc;
    int epoll_ctl_op;

    assert(iosvc);

    if (fd < 0)
        return;

    pthread_mutex_lock(&iosvc->mtx);

    if (!iosvc->allow_new_jobs) {
        pthread_mutex_unlock(&iosvc->mtx);
        return;
    }

    atn_fd = avl_tree_add_or_get(&iosvc->fd_map, fd, &inserted);

    assert(atn_fd && atn_fd->data);

    fd_desc = atn_fd->data;

    if (inserted) {
        fd_desc->fd = fd;

        fd_desc->mask = 0;
        fd_desc->masked = true;

        memset(fd_desc->op, 0, sizeof(fd_desc->op));

        memset(&fd_desc->event, 0, sizeof(fd_desc->event));

        fd_desc->event.data.ptr = fd_desc;

        already_watched = false;
    }

    assert(fd_desc->masked);

    fd_desc->mask = mask;
    fd_desc->op[IO_SVC_OP_COUNT].cb.masked_op = f;
    fd_desc->op[IO_SVC_OP_COUNT].ctx = ctx;
    fd_desc->op[IO_SVC_OP_COUNT].oneshot = oneshot;

    fd_desc->event.events = 0;
    if (fd_desc->mask & IO_SVC_OP_READ_MASK)
        fd_desc->event.events |= OP_MAP[IO_SVC_OP_READ];
    if (fd_desc->mask & IO_SVC_OP_WRITE_MASK)
        fd_desc->event.events |= OP_MAP[IO_SVC_OP_WRITE];

    if (already_watched)
        epoll_ctl_op = EPOLL_CTL_MOD;
    else
        epoll_ctl_op = EPOLL_CTL_ADD;

    rc = epoll_ctl(iosvc->epoll_fd, epoll_ctl_op,
                   fd_desc->fd, &fd_desc->event);

    assert(0 == rc);
    DONT_USE(rc);

    pthread_mutex_unlock(&iosvc->mtx);
}

void io_service_unwatch_fd_masked(io_service_t *iosvc, int fd) {
    iosvc_fd_desc_t *fd_desc;
    avl_tree_node_t *atn_fd;
    int rc;
    int epoll_ctl_op;
    void *ptr;

    assert(iosvc);

    if (fd < 0)
        return;

    pthread_mutex_lock(&iosvc->mtx);

    atn_fd = avl_tree_get(&iosvc->fd_map, fd);

    if (!atn_fd) {
        pthread_mutex_unlock(&iosvc->mtx);
        return;
    }

    fd_desc = atn_fd->data;

    assert(fd_desc->masked);
    DONT_USE(fd_desc);

    epoll_ctl_op = EPOLL_CTL_DEL;
    ptr = NULL;

    avl_tree_remove(&iosvc->fd_map, fd);

    rc = epoll_ctl(iosvc->epoll_fd, epoll_ctl_op,
                   fd, ptr);

    assert(0 == rc);
    DONT_USE(rc);

    pthread_mutex_unlock(&iosvc->mtx);
}

void io_service_run(io_service_t *iosvc) {
#define EVENTS_NUMBER       10
#define ARRAY_SIZE(arr)     (sizeof(arr) / sizeof(arr[0]))
    struct epoll_event events[EVENTS_NUMBER];
    int rc;

    assert(iosvc);

    pthread_mutex_lock(&iosvc->mtx);

    while (iosvc->running && (iosvc->allow_new_jobs || (1 < iosvc->fd_map.count))) {
        pthread_mutex_unlock(&iosvc->mtx);

        rc = epoll_wait(iosvc->epoll_fd, events, ARRAY_SIZE(events), -1);

        assert(rc >= 0 && errno != EINTR);

        if (0 == rc) {
            pthread_mutex_lock(&iosvc->mtx);
            continue;
        }

        _run_events(iosvc, events, rc);

        pthread_mutex_lock(&iosvc->mtx);
    }

    pthread_mutex_unlock(&iosvc->mtx);
}
