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

/* declare unary expr wrappers and overload
 * arithmetic/logical operators for ir builder API. */
expr exp(expr);
expr operator+(expr, expr);
expr operator-(expr, expr);
expr operator*(expr, expr);
expr operator/(expr, expr);
expr operator%(expr, expr);
expr operator&&(expr, expr);
expr operator>(expr, expr);
expr operator>=(expr, expr);
expr operator<(expr, expr);

/* to_ranges construct array of range from shape. */
exprs to_ranges(dimensions);

/* to_shape construct dimensions from ranges. */
dimensions to_shape(exprs);

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

} // namespace tcc

#endif // TCC_CORE_IR_UTIL_H
