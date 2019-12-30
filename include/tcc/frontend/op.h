#ifndef TCC_FRONTEND_OP
#define TCC_FRONTEND_OP

#include "tcc/common/datatype.h"
#include "tcc/hlir/ir.h"
#include <vector>

namespace tcc {
namespace frontend {

hlir::expr
build_op_placeholder(data_type, std::vector<long>);

hlir::expr
build_op_const(const std::string, data_type, std::vector<long>);

hlir::expr
build_op_add(hlir::expr&, hlir::expr&);

} // namespace frontend
} // namespace tcc

#endif // TCC_FRONTEND_OP
