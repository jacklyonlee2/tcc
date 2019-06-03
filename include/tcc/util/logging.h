#ifndef TCC_LOGGING_H
#define TCC_LOGGING_H

#include "glog/logging.h"

#define CHECK_KEY_IN_MAP(key, map) \
    CHECK((map).find(key) != (map).end()) << \
        "Key '" << key << "' is not found in map"

#define CHECK_KEY_NOT_IN_MAP(key, map) \
    CHECK((map).find(key) == (map).end()) << \
        "Key '" << key << "' is already exists in map"

#endif // TCC_LOGGING_H
