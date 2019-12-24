#include "tcc/frontend/op.h"
#include "tcc/common/logging.h"

namespace tcc {
namespace frontend {

hlir::expr
parse_op_add(hlir::expr& x, hlir::expr& y)
{
    tcc_assert_not_null(x);
    tcc_assert_not_null(y);

    return nullptr;
}

} // namespace frontend
} // namespace tcc
