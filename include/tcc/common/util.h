#ifndef TCC_COMMON_UTIL_H
#define TCC_COMMON_UTIL_H

#include "tcc/common/datatype.h"
#include "tcc/common/logging.h"
#include <sstream>
#include <string.h>
#include <vector>

namespace std {

inline string to_string(const void* val)
{
    stringstream ss;
    ss << val;
    return ss.str();
}

inline string to_string(tcc::data_type val)
{
    switch (val)
    {
        case tcc::data_type::BOOL:
            return "BOOL";
        case tcc::data_type::FP32:
            return "FP32";
        case tcc::data_type::INT32:
            return "INT32";
        case tcc::data_type::INT64:
            return "INT64";
        default:
            tcc_error("unknown data type.");
    }
}

} // namespace std

namespace tcc {

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
    return std::string(p, p + vec.size() * sizeof(T));
}

template<typename T>
std::vector<T> vector_deserialize(std::string str, size_t size)
{
    T* p = reinterpret_cast<T*>(strdup(str.data()));
    return std::vector<T>(p, p + size);
}

inline dimensions boardcast(dimensions x, dimensions y)
{
    tcc_assert(x.size() >= y.size(), "y can not be boardcasted to x.");

    dimensions boardcasted(x.size() - y.size(), 1);
    boardcasted.insert(boardcasted.end(), y.begin(), y.end());

    for (unsigned i = 0; i < x.size(); i++)
    {
        if (x[i] == boardcasted[i] || boardcasted[i] == 1)
        {
            boardcasted[i] = x[i];
        }
        else
        {
            tcc_error("dimension " + std::to_string(i) +
                      " of x and y are not boardcastable.");
        }
    }

    return boardcasted;
}

} // namespace tcc

#endif // TCC_COMMON_UTIL_H
