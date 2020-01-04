#ifndef TCC_FRONTEND_OP
#define TCC_FRONTEND_OP

#include "tcc/common/datatype.h"
#include "tcc/affn/ir.h"

namespace tcc {
namespace frontend {

affn::expr build_placeholder(data_type, dimensions);

affn::expr build_const(std::string, data_type, dimensions);

affn::expr build_add(affn::expr, affn::expr);

affn::expr build_avgpool(std::string,
                            std::string,
                            dimensions,
                            dimensions,
                            affn::expr);

affn::expr build_biasadd(std::string, affn::expr, affn::expr);

affn::expr build_conv2d(std::string,
                           std::string,
                           dimensions,
                           dimensions,
                           affn::expr,
                           affn::expr);

affn::expr build_depthwiseconv2dnative(std::string,
                                          std::string,
                                          dimensions,
                                          dimensions,
                                          affn::expr,
                                          affn::expr);

affn::expr build_fusedbatchnorm(float,
                                   std::string,
                                   affn::expr,
                                   affn::expr,
                                   affn::expr,
                                   affn::expr,
                                   affn::expr);

affn::expr build_relu6(affn::expr);

affn::expr build_reshape(affn::expr, affn::expr);

affn::expr build_shape(affn::expr);

affn::expr build_softmax(affn::expr);

affn::expr build_squeeze(dimensions, affn::expr);

} // namespace frontend
} // namespace tcc

#endif // TCC_FRONTEND_OP
