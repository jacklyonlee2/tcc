#include "tcc/frontend/op.h"
#include "tcc/common/logging.h"

using namespace tcc::hlir;

namespace tcc {
namespace frontend {

expr build_placeholder(data_type dtype, std::vector<long> shape)
{
    return var::make(dtype, shape);
}

expr build_const(const std::string data,
                 data_type dtype,
                 std::vector<long> shape)
{
    return cnst::make(data, dtype, shape);
}

expr build_add(expr x, expr y)
{
    return x + y;
}

expr build_avgpool(std::string data_format,
                   std::string padding,
                   std::vector<long> ksize,
                   std::vector<long> strides,
                   expr value)
{}

expr build_biasadd(std::string data_format, expr input, expr bias) {}

expr build_conv2d(std::string data_format,
                  std::string padding,
                  std::vector<long> strides,
                  std::vector<long> dilations,
                  expr input,
                  expr filter)
{}

expr build_depthwiseconv2dnative(std::string data_format,
                                 std::string padding,
                                 std::vector<long> strides,
                                 std::vector<long> dilations,
                                 expr input,
                                 expr filter)
{}

expr build_fusedbatchnorm(float epsilon,
                          std::string data_format,
                          expr x,
                          expr scale,
                          expr offset,
                          expr mean,
                          expr variance)
{}

expr build_relu6(expr features) {}

expr build_reshape(expr tensor, expr shape) {}

expr build_shape(expr input) {}

expr build_softmax(expr logits) {}

expr build_squeeze(std::vector<long> sqeeuze_dims, expr input) {}

} // namespace frontend
} // namespace tcc
