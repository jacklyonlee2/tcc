#ifndef TCC_LLIR_IR
#define TCC_LLIR_IR

#include <memory>
#include <vector>

namespace tcc {
namespace llir {

enum class blk_type
{
};

/* blk_base is the base class all llir primitives derive from.
 * blk_base stores the blk_type of the derived class and
 * "accept" virtual method to support visitor design pattern. */
struct blk_base
{
    blk_base(blk_type t)
        : type(t)
    {}

    blk_type type;
};

/* blk_template is a template class
 * used to declare llir primitives. */
template<typename T>
struct blk_template
    : blk_base
    , std::enable_shared_from_this<blk_template<T>>
{
    blk_template()
        : blk_base(T::btype)
    {}
};

/* aliases for pointers of blk_base. */
typedef std::shared_ptr<const blk_base> blk;
typedef std::vector<blk> blks;

} // namespace llir
} // namespace tcc

#endif // TCC_LLIR_IR
