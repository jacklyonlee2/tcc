#ifndef TCC_FRONTEND_OP_H
#define TCC_FRONTEND_OP_H

#include "tcc/common/datatype.h"
#include "tcc/core/ir.h"

namespace tcc {
namespace frontend {

core::expr build_placeholder(data_type, dimensions);

core::expr build_const(std::string, data_type, dimensions);

core::expr build_add(core::expr, core::expr);

core::expr build_avgpool(std::string,
                            std::string,
                            std::vector<int64_t>,
                            std::vector<int64_t>,
                            core::expr);

core::expr build_biasadd(std::string, core::expr, core::expr);

core::expr build_conv2d(std::string,
                           std::string,
                           std::vector<int64_t>,
                           std::vector<int64_t>,
                           core::expr,
                           core::expr);

core::expr build_depthwiseconv2dnative(std::string,
                                          std::string,
                                          std::vector<int64_t>,
                                          std::vector<int64_t>,
                                          core::expr,
                                          core::expr);

core::expr build_fusedbatchnorm(float,
                                   std::string,
                                   core::expr,
                                   core::expr,
                                   core::expr,
                                   core::expr,
                                   core::expr);

core::expr build_relu6(core::expr);

core::expr build_reshape(core::expr, core::expr);

core::expr build_shape(core::expr);

core::expr build_softmax(core::expr);

core::expr build_squeeze(std::vector<int64_t>, core::expr);

} // namespace frontend
} // namespace tcc

#endif // TCC_FRONTEND_OP_H
