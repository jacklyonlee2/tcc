#ifndef TCC_UTIL_H
#define TCC_UTIL_H

#include <iostream>
#include <vector>

namespace tcc {
namespace core {

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    // Overloading << to log vectors
    os << "[";
    for (unsigned int i = 0; i < v.size(); ++i) {
        os << v[i];
        if (i != v.size() - 1)
            os << ", ";
    }
    os << "]\n";
    return os;
}

template <typename... Args>
std::vector<typename std::common_type<Args...>::type> Literal(Args&&... args) {
    return {args...};
}

} // namespace core
} // namespace tcc

#endif // TCC_UTIL_H
