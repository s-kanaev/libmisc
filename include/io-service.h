#ifndef _IO_SERVICE_H_
# define _IO_SERVICE_H_

# include "containers.h"
# include "avl-tree.h"

# include <stdbool.h>
# include <pthread.h>
# include <sys/epoll.h>

struct io_service;
typedef struct io_service io_service_t;

struct iosvc_op_desc;
typedef struct iosvc_op_desc iosvc_op_desc_t;

struct iosvc_fd_desc;
typedef struct iosvc_fd_desc iosvc_fd_desc_t;

struct iosvc_enqueued_op;
typedef struct iosvc_enqueued_op iosvc_enqueued_op_t;

enum io_service_operation;

typedef void (*iosvc_fd_op_t)(int fd, enum io_service_operation op,
                              io_service_t *iosvc, void *ctx);
typedef void (*iosvc_fd_masked_op_t)(int fd, int mask,
                                     io_service_t *iosvc, void *ctx);
typedef void (*iosvc_enqueued_op_cb_t)(io_service_t *iosvc, void *ctx);

enum io_service_operation {
    IO_SVC_OP_READ = 0,
    IO_SVC_OP_WRITE,
    IO_SVC_OP_MAX = IO_SVC_OP_WRITE,
    IO_SVC_OP_COUNT,
};

enum io_service_operation_mask {
    IO_SVC_OP_READ_MASK = 0x01 << IO_SVC_OP_READ,
    IO_SVC_OP_WRITE_MASK = 0x01 << IO_SVC_OP_WRITE
};

struct iosvc_op_desc {
    bool oneshot;

    union {
        iosvc_fd_op_t op;
        iosvc_fd_masked_op_t masked_op;
    } cb;
    void *ctx;
};

struct iosvc_fd_desc {
    int fd;

    bool masked;
    int mask;

    struct epoll_event event;

    iosvc_op_desc_t op[IO_SVC_OP_COUNT + 1];    /* one more for masked */
};

struct iosvc_enqueued_op {
    iosvc_enqueued_op_cb_t cb;
    void *ctx;
    bool fresh;
};

struct io_service {
    /* fd -> iosvc_fd_desc_t */
    avl_tree_t fd_map;
    list_t enqueued_ops;

    bool running;
    bool allow_new_jobs;
    bool running_enqueued;

    int event_fd;
    int epoll_fd;

    pthread_mutex_t mtx;
};

/******************************* API *******************************/
void io_service_init(io_service_t *iosvc);
void io_service_enqueue_function(io_service_t *iosvc,
                                 iosvc_enqueued_op_cb_t f, void *ctx);
void io_service_watch_fd(io_service_t *iosvc,
                         int fd, enum io_service_operation op,
                         iosvc_fd_op_t f, void *ctx,
                         bool oneshot);
void io_service_unwatch_fd(io_service_t *iosvc,
                           int fd, enum io_service_operation op);
void io_service_watch_fd_masked(io_service_t *iosvc,
                                int fd, int mask,
                                iosvc_fd_masked_op_t f, void *ctx,
                                bool oneshot);
void io_service_unwatch_fd_masked(io_service_t *iosvc,
                                  int fd);
void io_service_deinit(io_service_t *iosvc);
void io_service_run(io_service_t *iosvc);
void io_service_stop(io_service_t *iosvc, bool wait_pending);

#endif /* _IO_SERVICE_H_ */
