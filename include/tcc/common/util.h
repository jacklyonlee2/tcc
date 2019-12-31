#ifndef TCC_COMMON_UTIL_H
#define TCC_COMMON_UTIL_H

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
    T* p = reinterpret_cast<T*>(str);
    return *p;
}

inline std::vector<long> boardcast(std::vector<long> x, std::vector<long> y)
{
    tcc_assert(x.size() >= y.size(), "y can not be boardcasted to x.");

    std::vector<long> boardcasted(x.size() - y.size(), 1);
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
