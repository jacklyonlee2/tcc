#ifndef TCC_COMMON_DATA_H
#define TCC_COMMON_DATA_H

#include "tcc/common/logging.h"
#include <cstdint>
#include <string.h>
#include <typeinfo>
#include <vector>

namespace tcc {

enum class datatype
{
    BOOL,  // bool
    FP32,  // float
    INT32, // int32_t
    INT64, // int64_t
};

template<typename T>
datatype to_datatype()
{
    if (typeid(T) == typeid(float))
    {
        return datatype::FP32;
    }
    else if (typeid(T) == typeid(int64_t))
    {
        return datatype::INT64;
    }
    else
    {
        tcc_error("unsupported C++ type.");
    }
}

template<typename T>
std::string scalar_serialize(T data)
{
    char* p = reinterpret_cast<char*>(&data);
    return std::string(p, p + sizeof(data));
}

template<typename T>
T scalar_deserialize(std::string str)
{
    T* p = reinterpret_cast<T*>(const_cast<char*>(str.data()));
    return *p;
}

template<typename T>
std::string vector_serialize(std::vector<T> vec)
{
    char* p = reinterpret_cast<char*>(vec.data());
    return std::string(p, p + vec.size());
}

template<typename T>
std::vector<T> vector_deserialize(std::string str, size_t size)
{
    T* p = reinterpret_cast<T*>(strdup(str.data()));
    return std::vector<T>(p, p + size);
}

typedef int64_t dimension;
typedef std::vector<dimension> dimensions;

inline dimensions boardcast(dimensions x, dimensions y)
{
    tcc_assert(x.size() >= y.size(), "y can not be boardcasted to x.");

    unsigned diff = x.size() - y.size();
    for (unsigned i = diff; i < x.size(); i++)
    {
        if (x[i] != y[i - diff] && y[i - diff] != 1)
        {
            tcc_error("y can not be boardcasted to x.");
        }
    }

    return x;
}

} // namespace tcc

#endif // TCC_COMMON_DATA_H
