#ifndef TCC_CORE_IR_UTIL_H
#define TCC_CORE_IR_UTIL_H

#include "tcc/core/ir_visitor.h"

namespace tcc {

/* downcast casts expr to core expr. */
template<typename T>
std::shared_ptr<const T> downcast(expr e)
{
    tcc_assert(e && e->type == T::expr_type, "illegal downcast.");
    return std::static_pointer_cast<const T>(e);
}

/* to_ranges construct ranges from shape. */
inline exprs to_ranges(dimensions shape)
{
    exprs ranges;
    for (dimension dim : shape)
    {
        ranges.push_back(range::make(dim));
    }
    return ranges;
}

/* to_shape construct shape from ranges. */
inline dimensions to_shape(exprs ranges)
{
    dimensions shape;
    for (expr e : ranges)
    {
        shape.push_back(downcast<range>(e)->bound);
    }
    return shape;
}

/* index_validator ensures all range exprs reachable from
 * the given indices are contained within the given ranges. */
struct index_validator : ir_visitor
{
    /* returns true if indices are valid, false otherwise. */
    static bool apply(exprs, exprs);

  protected:
    void visit(range_expr) override;

    std::unordered_set<expr> valid_ranges;
    bool found_invalid_range = false;
};

/* declare unary expr wrappers and overload
 * arithmetic/logical operators for ir builder API. */
expr exp(expr);
expr sqrt(expr);
expr operator+(expr, expr);
expr operator-(expr, expr);
expr operator*(expr, expr);
expr operator/(expr, expr);
expr operator%(expr, expr);
expr operator>(expr, expr);
expr operator>=(expr, expr);
expr operator<(expr, expr);
expr operator&&(expr, expr);

} // namespace tcc

#endif // TCC_CORE_IR_UTIL_H
