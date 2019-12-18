#ifndef TCC_COMMON_LOGGING_H
#define TCC_COMMON_LOGGING_H

#define __tcc_log(fd, type, fmt, ...)                                          \
    fprintf(fd, "%s:%d:%s " fmt "\n", __FILE__, __LINE__, type, __VA_ARGS__)

#define tcc_error(fmt, ...)                                                    \
    do                                                                         \
    {                                                                          \
        __tcc_log(stderr, "error", fmt, __VA_ARGS__);                          \
        abort();                                                               \
                                                                               \
    } while (0)

#ifdef NDEBUG

#define tcc_assert(expr, fmt, ...) ((void)0)

#define tcc_info(fmt, ...) ((void)0)

#else // NDEBUG

#define tcc_assert(expr, fmt, ...)                                             \
    do                                                                         \
    {                                                                          \
        if (!(expr))                                                           \
        {                                                                      \
            __tcc_log(stderr, "assert(" #expr ")", fmt, __VA_ARGS__);          \
            abort();                                                           \
        }                                                                      \
    } while (0)

#define tcc_info(fmt, ...)                                                     \
    do                                                                         \
    {                                                                          \
        __tcc_log(stdout, "info", fmt, __VA_ARGS__);                           \
    } while (0)

#endif // NDEBUG

#endif // TCC_COMMON_LOGGING_H
