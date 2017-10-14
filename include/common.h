#ifndef _LIB_COMMON_H_
# define _LIB_COMMON_H_

# include <errno.h>
# include <assert.h>

# define LIBMISC_WARN_UNUSED_RET    __attribute__((warn_unused_result))
# define LIBMISC_UNUSED             __attribute__((unused))
# define LIBMISC_DONT_USE(x)        (void)(x)

# define LIBMISC_INIT_RETURN_ERROR_VALUE    (LIBMISC_INIT_RETURN_TYPE)(-1)
# define LIBMISC_INIT_RETURN_SUCCESS_VALUE  (LIBMISC_INIT_RETURN_TYPE)(0)

# define LIBMISC_INIT_RETURN_IS_SUCCESS(x)      \
    (LIBMISC_INIT_RETURN_SUCCESS_VALUE == (x))

# define LIBMISC_INIT_RETURN_IS_ERROR(x)        \
    (LIBMISC_INIT_RETURN_ERROR_VALUE == (x))

# define LIBMISC_INIT_RETURN_ERROR(err)         \
do {                                            \
    errno = (err);                              \
    return LIBMISC_INIT_RETURN_ERROR_VALUE;     \
} while (0)

# define LIBMISC_INIT_RETURN_SAME_ERROR()       \
do {                                            \
    return LIBMISC_INIT_RETURN_ERROR_VALUE;     \
} while (0)

# define LIBMISC_INIT_RETURN_SUCCESS()          \
do {                                            \
    return LIBMISC_INIT_RETURN_SUCCESS_VALUE;   \
} while (0)

# define LIBMISC_MAKE_ASSERTION_OR_ACT(assertion, action)   \
do {                                                        \
    assert((assertion));                                    \
                                                            \
    if (!(assertion)) {                                     \
        action;                                             \
    }                                                       \
} while (0)

typedef int LIBMISC_INIT_RETURN_TYPE;
typedef int LIBMISC_BOOL_TYPE;

# define LIBMISC_TRUE               (LIBMISC_BOOL_TYPE)(1)
# define LIBMISC_FALSE              (LIBMISC_BOOL_TYPE)(0)
# define LIBMISC_BOOL_IS_TRUE(x)    (LIBMISC_TRUE == (x))
# define LIBMISC_BOOL_IS_FALSE(x)   (LIBMISC_FALSE == (x))
# define LIBMISC_AS_BOOL(x)         ((x) ? LIBMISC_TRUE : LIBMISC_FALSE)

#endif /* _LIB_COMMON_H_ */
