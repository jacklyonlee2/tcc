#ifndef TCC_FRONTEND_OP
#define TCC_FRONTEND_OP

#include "tcc/common/datatype.h"
#include "tcc/hlir/ir.h"
#include <vector>

namespace tcc {
namespace frontend {

hlir::expr build_placeholder(data_type, std::vector<long>);

hlir::expr build_const(const std::string, data_type, std::vector<long>);

hlir::expr build_add(hlir::expr, hlir::expr);

hlir::expr build_avgpool(std::string,
                         std::string,
                         std::vector<long>,
                         std::vector<long>,
                         hlir::expr);

hlir::expr build_biasadd(std::string, hlir::expr, hlir::expr);

hlir::expr build_conv2d(std::string,
                        std::string,
                        std::vector<long>,
                        std::vector<long>,
                        hlir::expr,
                        hlir::expr);

hlir::expr build_depthwiseconv2dnative(std::string,
                                       std::string,
                                       std::vector<long>,
                                       std::vector<long>,
                                       hlir::expr,
                                       hlir::expr);

hlir::expr build_fusedbatchnorm(float,
                                std::string,
                                hlir::expr,
                                hlir::expr,
                                hlir::expr,
                                hlir::expr,
                                hlir::expr);

hlir::expr build_relu6(hlir::expr);

hlir::expr build_reshape(hlir::expr, hlir::expr);

hlir::expr build_shape(hlir::expr);

hlir::expr build_softmax(hlir::expr);

hlir::expr build_squeeze(std::vector<long>, hlir::expr);

} // namespace frontend
} // namespace tcc

#endif // TCC_FRONTEND_OP
