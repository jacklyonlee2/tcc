#ifndef TCC_COMMON_LOGGING_H
#define TCC_COMMON_LOGGING_H

#include "glog/logging.h"

/* Logging macros */

#define CHECK_KEY_IN_MAP(key, map) \
    CHECK((map).find(key) != (map).end())

#define CHECK_NOT_EXPIRED(weak_ptr) \
    CHECK(!(weak_ptr).expired())

#endif // TCC_COMMON_LOGGING_H
