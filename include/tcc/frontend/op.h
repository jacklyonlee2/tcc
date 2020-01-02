#ifndef TCC_FRONTEND_OP
#define TCC_FRONTEND_OP

#include "tcc/common/datatype.h"
#include "tcc/hlir/ir.h"
#include <vector>

namespace tcc {
namespace frontend {

hlir::expr parse_op_placeholder(data_type, dimensions);

hlir::expr parse_op_const(std::string, data_type, dimensions);

hlir::expr parse_op_add(hlir::expr, hlir::expr);

hlir::expr parse_op_avgpool(std::string,
                            std::string,
                            dimensions,
                            dimensions,
                            hlir::expr);

hlir::expr parse_op_biasadd(std::string, hlir::expr, hlir::expr);

hlir::expr parse_op_conv2d(std::string,
                           std::string,
                           dimensions,
                           dimensions,
                           hlir::expr,
                           hlir::expr);

hlir::expr parse_op_depthwiseconv2dnative(std::string,
                                          std::string,
                                          dimensions,
                                          dimensions,
                                          hlir::expr,
                                          hlir::expr);

hlir::expr parse_op_fusedbatchnorm(float,
                                   std::string,
                                   hlir::expr,
                                   hlir::expr,
                                   hlir::expr,
                                   hlir::expr,
                                   hlir::expr);

hlir::expr parse_op_relu6(hlir::expr);

hlir::expr parse_op_reshape(hlir::expr, hlir::expr);

hlir::expr parse_op_shape(hlir::expr);

hlir::expr parse_op_softmax(hlir::expr);

hlir::expr parse_op_squeeze(dimensions, hlir::expr);

} // namespace frontend
} // namespace tcc

#endif // TCC_FRONTEND_OP
