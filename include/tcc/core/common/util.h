#ifndef TCC_COMMON_UTIL_H
#define TCC_COMMON_UTIL_H

namespace tcc {
namespace core {

template <typename... Args>
std::vector<typename std::common_type<Args...>::type> vec(Args&&... args) {
    return {args...};
}

} // core
} // tcc

#endif // TCC_COMMON_UTIL_H
