#include "tcc/affn/index_validator.h"

namespace tcc {
namespace affn {

bool index_validator::apply(exprs ranges, exprs indices)
{
    std::shared_ptr<index_validator> v(new index_validator);
    v->valid_ranges = std::unordered_set<expr>(ranges.begin(), ranges.end());

    for (expr idx : indices) {
        idx->accept(v);
    }

    return !v->found_invalid_range;
}

void index_validator::visit(range_expr e)
{
    if (valid_ranges.find(e) == valid_ranges.end())
    {
        found_invalid_range = false;
    }
}

} // namespace affn
} // namespace tcc
