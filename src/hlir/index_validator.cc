#include "tcc/hlir/index_validator.h"

namespace tcc {
namespace hlir {

bool index_validator::apply(exprs ranges, exprs indices)
{
    std::shared_ptr<index_validator> v(new index_validator);

    for (expr e : ranges)
    {
        v->valid_ranges.insert(downcast<range>(e));
    }

    v->valid = true;

    static_cast<visitor>(v)->visit(indices);

    return v->valid;
}

void index_validator::visit(range_expr e)
{
    if (valid_ranges.find(e) == valid_ranges.end())
    {
        valid = false;
    }
}

} // namespace hlir
} // namespace tcc
