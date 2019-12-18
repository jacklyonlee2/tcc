#ifndef TCC_HLIR_IR_H
#define TCC_HLIR_IR_H

#include <memory>

namespace tcc {
namespace hlir {

enum class expr_type
{
    var,
    const_,
    exp,
    sqrt,
    add,
    sub,
    mul,
    div,
    mod,
    greater,
    greater_equal,
    less,
    and_,
    select,
    reduce
};

struct base_expr
{
    base_expr(expr_type et)
        : type(et)
    {}

    expr_type type;
};

typedef std::shared_ptr<const base_expr> expr;

} // namespace hlir
} // namespace tcc

#endif // TCC_HLIR_IR_H
