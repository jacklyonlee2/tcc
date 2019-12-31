#include "tcc/hlir/index_validator.h"

namespace tcc {
namespace hlir {

visitor index_validator::make(ranges rs)
{
    std::shared_ptr<index_validator> v(new index_validator);
    
    for (expr r : rs)
    {
        v->valid_ranges.insert(downcast<range>(r));
    }

    return v;
}

void index_validator::visit(range_expr e)
{
    tcc_assert(valid_ranges.find(e) != valid_ranges.end(),
               "unspecified range expr is found.");
}

} // namespace hlir
} // namespace tcc
