#include "tcc/hlir/ir.h"
#include "tcc/hlir/visitor.h"

namespace tcc {
namespace hlir {

/* below are overridden expr_template accept methods
 * for each hlir primitive. */

template<>
void
expr_template<var>::accept(visitor* v) const
{
    v->visit(downcast<var>(shared_from_this()));
}

template<>
void
expr_template<cnst>::accept(visitor* v) const
{
    v->visit(downcast<cnst>(shared_from_this()));
}

/* below are derived expr's make implementations. */

expr
var::make(data_type dtype, std::vector<long> shape)
{
    tcc_assert(dtype != data_type::VOID, "dtype is uninitialized.");
    tcc_assert(!shape.empty(), "shape is empty.");

    std::shared_ptr<var> e(new var);
    e->dtype = dtype;
    e->shape = shape;
    return e;
}

expr
cnst::make(const std::string data, data_type dtype, std::vector<long> shape)
{
    tcc_assert(!data.empty(), "data is empty.");
    tcc_assert(dtype != data_type::VOID, "dtype is uninitialized.");
    tcc_assert(!shape.empty(), "shape is empty.");

    std::shared_ptr<cnst> e(new cnst);
    e->data = data;
    e->dtype = dtype;
    e->shape = shape;
    return e;
}

} // namespace hlir
} // namespace tcc
