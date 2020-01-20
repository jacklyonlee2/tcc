#ifndef TCC_COMMON_DATATYPE_H
#define TCC_COMMON_DATATYPE_H

#include <cstdint>
#include <vector>

namespace tcc {

enum class datatype
{
    BOOL,  // bool
    FP32,  // float
    INT32, // int32_t
    INT64, // int64_t
};

typedef int64_t dimension;
typedef std::vector<dimension> dimensions;

} // namespace tcc

#endif // TCC_COMMON_DATATYPE_H
