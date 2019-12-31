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
    tcc_assert(x.size() == y.size(), "ranks of x and y do not agree.");

    std::vector<long> boardcasted;
    for (unsigned i = 0; i < x.size(); i++)
    {
        if (x[i] > y[i] && y[i] == 1)
        {
            boardcasted.push_back(x[i]);
        }
        else if (y[i] > x[i] && x[i] == 1)
        {
            boardcasted.push_back(y[i]);
        }
        else if (x[i] == y[i])
        {
            boardcasted.push_back(x[i]);
        }
        else
        {
            tcc_error("dimension of " + std::to_string(x[i]) + " and " +
                      std::to_string(y[i]) + " are not boardcastable.");
        }
    }

    return boardcasted;
}

} // namespace tcc

#endif // TCC_COMMON_UTIL_H
