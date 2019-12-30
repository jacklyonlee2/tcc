#ifndef TCC_HLIR_IR_H
#define TCC_HLIR_IR_H

#include "tcc/common/datatype.h"
#include "tcc/common/logging.h"
#include <memory>
#include <vector>

namespace tcc {
namespace hlir {

enum class expr_type
{
    var,
    cnst,
    exp,
    sqrt,
    add,
    sub,
    mul,
    div,
    mod,
    greater_than,
    greater_than_or_equal_to,
    less_than,
    logical_and,
    select,
    reduce
};

/* forward declare hlir visitor class. */
class visitor;

/* expr_base is the base class all hlir primitives derive from.
 * expr_base stores the expr_type of the derived class, the
 * shape of its output and "accept" virtual method to support
 * visitor design pattern. */
struct expr_base
{
    expr_base(expr_type t)
        : type(t)
    {}

    virtual void accept(visitor* v) const = 0;

    expr_type type;
    mutable data_type dtype = data_type::VOID;
    mutable std::vector<long> shape;
};

typedef std::shared_ptr<const expr_base> expr;

/* expr_template is a template class
 * used to declare hlir primitives. */
template<typename T>
struct expr_template
    : expr_base
    , std::enable_shared_from_this<expr_template<T>>
{
    expr_template()
        : expr_base(T::etype)
    {}

    void accept(visitor* v) const override;
};

/* below are hlir primitive declarations.
 * when declaring a new hlir primitive:
 * * add the respective expr_type to the expr_type enum class;
 * * declare the struct for the respective hlir primitive deriving
 *   for the expr_template class;
 * * declare and implement "make" method used for initialization
 *   in place of the default constructor;
 * * declare "etype" field initialized to the respective expr_type;
 * * add a new type alias for pointers to the respective hlir primitive;
 * * add a new visit method to the hlir visitor class and all of
 *   its derived classes for ther respective hlir primitive.
 * * override the respective "accept" method of expr_template. */

struct var : expr_template<var>
{
    static expr make(data_type, std::vector<long>);

    static const expr_type etype = expr_type::var;
};

struct cnst : expr_template<cnst>
{
    std::string data;

    static expr make(const std::string, data_type, std::vector<long>);

    static const expr_type etype = expr_type::cnst;
};

/* below are aliases for pointers to hlir primitives. */

typedef std::shared_ptr<const var> var_expr;
typedef std::shared_ptr<const cnst> cnst_expr;

/* helper function downcast casts expr to derived expr. */
template<typename T>
std::shared_ptr<const T>
downcast(expr e)
{
    tcc_assert(e && e->type == T::etype, "illegal downcast of hlir expr.");
    return std::static_pointer_cast<const T>(e);
}

} // namespace hlir
} // namespace tcc

#endif // TCC_HLIR_IR_H
