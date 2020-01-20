#ifndef TCC_CORE_IR_UTIL_H
#define TCC_CORE_IR_UTIL_H

#include "tcc/core/ir_visitor.h"

namespace tcc {
namespace core {

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

} // namespace core
} // namespace tcc

#endif // TCC_CORE_IR_UTIL_H
