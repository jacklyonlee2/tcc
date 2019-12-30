#include "tcc/frontend/op.h"
#include "tcc/common/logging.h"

using namespace tcc::hlir;

namespace tcc {
namespace frontend {

expr
build_op_placeholder(data_type dtype, std::vector<long> shape)
{
    return var::make(dtype, shape);
}

expr
build_op_const(const std::string data, data_type dtype, std::vector<long> shape)
{
    return cnst::make(data, dtype, shape);
}

expr
build_op_add(expr& x, expr& y)
{
    tcc_assert_not_null(x);
    tcc_assert_not_null(y);

    return nullptr;
}

} // namespace frontend
} // namespace tcc
