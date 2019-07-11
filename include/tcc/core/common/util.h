#ifndef TCC_COMMON_UTIL_H
#define TCC_COMMON_UTIL_H

#include <iostream>
#include <numeric>

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
    os << "]";
    return os;
}

/* Helper function to accumulate vector values. */
template<typename T> T accumulate_vector(std::vector<T> vec) {
    return std::accumulate(
            vec.begin(),
            vec.end(),
            static_cast<T>(1),
            std::multiplies<T>());
}

/* Helper function to accumulate parameter pack. */
template<typename T> std::vector<T> accumulate_parameters(T arg) { return { arg }; }
template<typename T, typename ... Args>
std::vector<T> accumulate_parameters(T arg, Args ... args) {
    std::vector<T> arg_vec({arg});
    std::vector<T> rest = accumulate_parameters(args...);
    arg_vec.insert(arg_vec.end(), rest.begin(), rest.end());
    return arg_vec;
}

} // core
} // tcc

#endif // TCC_COMMON_UTIL_H
