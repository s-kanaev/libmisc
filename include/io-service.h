#ifndef _IO_SERVICE_H_
# define _IO_SERVICE_H_

# include "containers.h"
# include "avl-tree.h"

# include <pthread.h>

struct io_service;
typedef struct io_service io_service_t;

struct iosvc_op_desc;
typedef struct iosvc_op_desc iosvc_op_desc_t;

struct iosvc_fd_desc;
typedef struct iosvc_fd_desc iosvc_fd_desc_t;

struct iosvc_enqueued_op;
typedef struct iosvc_enqueued_op iosvc_enqueued_ops_t;

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

struct iosvc_op_desc {
    union {
        iosvc_fd_op_t op;
        iosvc_fd_masked_op_t masked_op;
    } cb;
    void *ctx;
};

struct iosvc_fd_desc {
    int fd;

    bool masked;

    iosvc_op_desc_t op[IO_SVC_OP_COUNT + 1];    /* one more for masked */
};

struct iosvc_enqueued_ops_t {
    /* list of iosvc_enqueued_op_cb_t */
    list_t ops;
};

struct io_service {
    /* fd -> iosvc_fd_desc_t */
    avl_tree_t fd_map;
    iosvc_enqueued_ops_t enqueued_ops;

    int event_fd;

    pthread_mutexattr_t mtx_attr;
    pthread_mutex_t mtx;
};

#endif /* _IO_SERVICE_H_ */
