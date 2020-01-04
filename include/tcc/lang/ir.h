#ifndef TCC_LANG_IR
#define TCC_LANG_IR

#include <memory>
#include <vector>

namespace tcc {
namespace lang {

enum class prim_type
{
    scalar,
    array,
    unary,
    binary,
    loop,
    func
};

/* abstract base class for lang prims. */  
struct abstract_prim
{
    abstract_prim(prim_type t)
        : type(t)
    {}

    prim_type type;
};

/* base class for lang prims. */
template<typename T>
struct base_prim
    : abstract_prim
    , std::enable_shared_from_this<base_prim<T>>
{
    base_prim()
        : abstract_prim(T::btype)
    {}
};

typedef std::shared_ptr<const abstract_prim> prim;
typedef std::vector<prim> prims;

struct scalar : base_prim<scalar>
{
    static const prim_type btype = prim_type::scalar;
};

struct array : base_prim<array>
{
    static const prim_type btype = prim_type::array;
};

struct unary : base_prim<unary>
{
    enum class type
    {
        exp,
        sqrt
    };

    static const prim_type btype = prim_type::unary;
};

struct binary : base_prim<binary>
{
    enum class type
    {
        add,
        sub,
        mul,
        div
    };

    static const prim_type btype = prim_type::binary;
};

struct loop : base_prim<loop>
{
    prims body;

    static const prim_type btype = prim_type::loop;
};

struct func : base_prim<func>
{
    prims body;

    static const prim_type btype = prim_type::func;
};

typedef std::shared_ptr<const scalar> scalar_prim;
typedef std::shared_ptr<const array> array_prim;
typedef std::shared_ptr<const loop> loop_prim;

} // namespace lang
} // namespace tcc

#endif // TCC_LANG_IR
