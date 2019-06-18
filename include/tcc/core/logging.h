#ifndef TCC_LOGGING_H
#define TCC_LOGGING_H

#include <iostream>
#include <algorithm>

#include "glog/logging.h"

// Alternative macros that do not depend on glog API.
// Reason: glog CHECK macro segfaults at static initialization time.
#define NATIVE_CHECK(cond, msg) NATIVE_CHECK_HELPER(cond, msg, #cond, __FILE__, __LINE__)
#define NATIVE_CHECK_HELPER(cond, msg, cond_str, file, line) \
    if (!(cond)) { \
        std::cerr << file << ":" << line << " " << cond_str << std::endl << \
            "Assertion failed: " << msg << std::endl; \
        abort(); \
    }

#define NATIVE_CHECK_KEY_IN_MAP(key, map, msg) \
    NATIVE_CHECK((map).find(key) != (map).end(), msg)

#define NATIVE_CHECK_KEY_NOT_IN_MAP(key, map, msg) \
    NATIVE_CHECK((map).find(key) == (map).end(), msg)

#define NATIVE_CHECK_KEY_NOT_IN_VEC(key, vec, msg) \
    NATIVE_CHECK(std::find((vec).begin(), (vec).end(), key) == (vec).end(), msg)

// Macros based on glog API

#define CHECK_KEY_IN_MAP(key, map) \
    CHECK((map).find(key) != (map).end())

#define CHECK_KEY_NOT_IN_MAP(key, map) \
    CHECK((map).find(key) == (map).end())

#define CHECK_KEY_NOT_IN_VEC(key, vec) \
    CHECK(std::find((vec).begin(), (vec).end(), key) == (vec).end())

#define CHECK_WITHIN_BOUND(index, vec) \
    CHECK(index < (vec).size())

#define CHECK_NOT_EXPIRED(weak_ptr) \
    CHECK(!(weak_ptr).expired())

#endif // TCC_LOGGING_H
