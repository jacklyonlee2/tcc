#ifndef TCC_LOGGING_H
#define TCC_LOGGING_H

#include <iostream>

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

#define NATIVE_CHECK_KEY_IN_MAP(key, map) \
    NATIVE_CHECK((map).find(key) != (map).end(), \
            "Key '" + key + "' is not found in " + #map + ".")

#define NATIVE_CHECK_KEY_NOT_IN_MAP(key, map) \
    NATIVE_CHECK((map).find(key) == (map).end(), \
             "Key '" + key + "' already exists in " + #map + ".")

// Macros based on glog API

#define CHECK_KEY_IN_MAP(key, map) \
    CHECK((map).find(key) != (map).end()) << \
        "Key '" << key << "' is not found in " << #map << "."

#endif // TCC_LOGGING_H
