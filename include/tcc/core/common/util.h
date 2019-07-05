#ifndef TCC_COMMON_UTIL_H
#define TCC_COMMON_UTIL_H

namespace tcc {
namespace core {

/* Overloading '<<' operator to print vectors. */
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    os << "[";
    for (unsigned int i = 0; i < v.size(); ++i) {
        os << v[i];
        if (i != v.size() - 1)
            os << ", ";
    }
    os << "]\n";
    return os;
}

/* Forwarding function to contruct vector literals for comparsion. */
template <typename... Args>
std::vector<typename std::common_type<Args...>::type> vec(Args&&... args) {
    return {args...};
}

} // core
} // tcc

#endif // TCC_COMMON_UTIL_H
