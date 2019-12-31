#ifndef TCC_COMMON_LOGGING_H
#define TCC_COMMON_LOGGING_H

#include <iostream>

#define __tcc_log(stream, prefix, msg)                                         \
    (stream) << __FILE__ << ":" << __LINE__ << ":" << (prefix) << " - "        \
             << (msg) << std::endl

#define tcc_error(msg)                                                         \
    do                                                                         \
    {                                                                          \
        __tcc_log(std::cerr, "error", msg);                                    \
        abort();                                                               \
                                                                               \
    } while (0)

#define tcc_assert(expr, msg)                                                  \
    do                                                                         \
    {                                                                          \
        if (!(expr))                                                           \
        {                                                                      \
            __tcc_log(std::cerr, "assert", msg);                               \
            abort();                                                           \
        }                                                                      \
    } while (0)

#define tcc_info(msg)                                                          \
    do                                                                         \
    {                                                                          \
        __tcc_log(std::cout, "info", msg);                                     \
    } while (0)

#define tcc_assert_has_key(map, key)                                           \
    tcc_assert((map).find((key)) != (map).end(),                               \
               "key \"" + std::string(key) +                                   \
                   "\" is not found in \"" #map "\".")

#define tcc_assert_no_key(map, key)                                            \
    tcc_assert((map).find((key)) == (map).end(),                               \
               "key \"" + std::string(key) +                                   \
                   "\" already exists in \"" #map "\".")

#define tcc_assert_not_null(ptr)                                               \
    tcc_assert((ptr) != nullptr, "\"" #ptr "\" can not be null.")

#define tcc_assert_size_eq(obj, sz)                                            \
    tcc_assert((obj).size() == (sz),                                           \
               "\"" #obj "\" has size " + std::to_string((obj).size()) +       \
                   "; expected size of " + std::to_string((sz)) + ".")

#endif // TCC_COMMON_LOGGING_H
