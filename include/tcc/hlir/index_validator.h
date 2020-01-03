#ifndef TCC_HLIR_INDEX_VALIDATOR_H
#define TCC_HLIR_INDEX_VALIDATOR_H

#include "tcc/hlir/visitor.h"

namespace tcc {
namespace hlir {

/* index_validate validate indices to ensure no unspecified
 * range exprs are contained within the index exprs. */
struct index_validator : visitor_base
{
    static bool apply(exprs, exprs);

  protected:
    void visit(range_expr) override;

    std::unordered_set<range_expr> valid_ranges;
    bool valid = true;
};

} // namespace hlir
} // namespace tcc

#endif // TCC_HLIR_INDEX_VALIDATOR_H
