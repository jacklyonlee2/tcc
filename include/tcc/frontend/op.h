#ifndef TCC_FRONTEND_OP_H
#define TCC_FRONTEND_OP_H

#include "tcc/common/datatype.h"
#include "tcc/core/ir.h"

namespace tcc {

expr build_placeholder(datatype, dimensions);

expr build_const(std::string, datatype, dimensions);

expr build_add(expr, expr);

expr build_avgpool(std::string, std::string, dimensions, dimensions, expr);

expr build_biasadd(std::string, expr, expr);

expr build_conv2d(std::string, std::string, dimensions, dimensions, expr, expr);

expr build_depthwiseconv2dnative(std::string,
                                 std::string,
                                 dimensions,
                                 dimensions,
                                 expr,
                                 expr);

expr build_fusedbatchnorm(float, std::string, expr, expr, expr, expr, expr);

expr build_relu6(expr);

expr build_reshape(expr, expr);

expr build_shape(expr);

expr build_softmax(expr);

expr build_squeeze(dimensions, expr);

} // namespace tcc

#endif // TCC_FRONTEND_OP_H
