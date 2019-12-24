#ifndef TCC_FRONTEND_OP
#define TCC_FRONTEND_OP

#include "tcc/hlir/ir.h"

namespace tcc {
namespace frontend {

hlir::expr
parse_op_add(hlir::expr&, hlir::expr&);

} // namespace frontend
} // namespace tcc

#endif // TCC_FRONTEND_OP
