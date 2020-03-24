#ifndef TCC_COMMON_DATA_H
#define TCC_COMMON_DATA_H

#include "tcc/common/logging.h"
#include <cstdint>
#include <sstream>
#include <typeinfo>
#include <vector>

namespace tcc {

typedef int64_t dimension;
typedef std::vector<dimension> dimensions;

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
    else if (typeid(T) == typeid(int64_t) || typeid(T) == typeid(long))
    {
        return datatype::INT64;
    }
    else
    {
        tcc_error("unsupported C++ type " + std::string(typeid(T).name()) + ".");
    }
}

template<typename T>
std::vector<T> vector_deserialize(std::string str, size_t size)
{
    static_assert(std::is_trivial<T>::value &&
                      std::is_standard_layout<T>::value,
                  "T must be a POD type.");

    std::vector<T> vec(size);
    std::stringstream ss(str);
    ss.read(reinterpret_cast<char*>(vec.data()), vec.size() * sizeof(T));
    return vec;
}

template<typename T>
std::string vector_serialize(std::vector<T> vec)
{
    static_assert(std::is_trivial<T>::value &&
                      std::is_standard_layout<T>::value,
                  "T must be a POD type.");

    std::stringstream ss;
    ss.write(reinterpret_cast<char const*>(vec.data()), vec.size() * sizeof(T));
    return ss.str();
}

template<typename T>
std::string scalar_serialize(T data)
{
    return vector_serialize<T>({ data });
}

template<typename T>
T scalar_deserialize(std::string str)
{
    return vector_deserialize<T>(str, 1)[0];
}

} // namespace tcc

#endif // TCC_COMMON_DATA_H
