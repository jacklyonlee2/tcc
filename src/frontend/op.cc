#include "tcc/frontend/op.h"
#include "tcc/common/logging.h"

using namespace tcc::hlir;

namespace tcc {
namespace frontend {

expr parse_op_placeholder(data_type dtype, std::vector<long> shape)
{
    return var::make(dtype, shape);
}

expr parse_op_const(const std::string data,
                    data_type dtype,
                    std::vector<long> shape)
{
    return cnst::make(data, dtype, shape);
}

expr parse_op_add(expr x, expr y)
{
    return x + y;
}

expr parse_op_avgpool(std::string data_format,
                      std::string padding,
                      std::vector<long> ksize,
                      std::vector<long> strides,
                      expr value)
{
    tcc_assert_not_null(value);
    tcc_assert(data_format != "NHWC",
               "\"" + data_format + "\" data format is not supported.");
    tcc_assert(padding != "VALID",
               "\"" + padding + "\" padding is not supported.");
    tcc_assert(ksize.size() == 4 && ksize[0] == 1 && ksize[3] == 1,
               "pooling along batch or channel dimension is not supported.");
    tcc_assert(strides.size() == 4 && strides[0] == 1 && strides[3] == 1,
               "strides along batch or channel dimension is not supported.");
    tcc_assert(value->shape.size() == 4, "value is not 4 dimensional.");

    long i_n, i_h, i_w, i_c;
    i_n = value->shape[0];
    i_h = value->shape[1];
    i_w = value->shape[2];
    i_c = value->shape[3];

    long k_h, k_w, s_h, s_w;
    k_h = ksize[1];
    k_w = ksize[2];
    s_h = strides[1];
    s_w = strides[2];

    long o_h, o_w;
    o_h = (i_h - k_h) / s_h + 1;
    o_w = (i_w - k_w) / s_w + 1;

    ranges i = to_ranges({ i_n, o_h, o_w, k_h, k_h, i_c });
    expr value_frag =
        select::make(i,
                     ((i[1] * cnst::make(s_h) + i[3] >= cnst::make(0l)) &&
                      (i[1] * cnst::make(s_h) + i[3] < cnst::make(i_h)) &&
                      (i[2] * cnst::make(s_w) + i[4] >= cnst::make(0l)) &&
                      (i[2] * cnst::make(s_w) + i[4] < cnst::make(i_w))),
                     index::make(i,
                                 value,
                                 { i[0],
                                   i[1] * cnst::make(s_h) + i[3],
                                   i[2] * cnst::make(s_w) + i[4],
                                   i[5] }),
                     cnst::make(0.0f));

    return reduce::make(reduce_type::avg, { 3, 4 }, value_frag);
}

expr parse_op_biasadd(std::string data_format, expr input, expr bias)
{
    tcc_assert_not_null(input);
    tcc_assert_not_null(bias);
    tcc_assert(data_format != "NHWC",
               "\"" + data_format + "\" data format is not supported.");
    tcc_assert(input->shape.size() == 4, "input is not 4 dimensional.");
    tcc_assert(bias->shape.size() == 1, "bias is not 1 dimensional.");
    tcc_assert(input->shape[3] == bias->shape[0],
               "input channel dimension and bias dimension do not agree.");

    return input + bias;
}

expr parse_op_conv2d(std::string data_format,
                     std::string padding,
                     std::vector<long> strides,
                     std::vector<long> dilations,
                     expr input,
                     expr filter)
{
    tcc_assert_not_null(input);
    tcc_assert_not_null(filter);
}

expr parse_op_depthwiseconv2dnative(std::string data_format,
                                    std::string padding,
                                    std::vector<long> strides,
                                    std::vector<long> dilations,
                                    expr input,
                                    expr filter)
{
    tcc_assert_not_null(input);
    tcc_assert_not_null(filter);
}

expr parse_op_fusedbatchnorm(float epsilon,
                             std::string data_format,
                             expr x,
                             expr scale,
                             expr offset,
                             expr mean,
                             expr variance)
{
    tcc_assert_not_null(x);
    tcc_assert_not_null(scale);
    tcc_assert_not_null(offset);
    tcc_assert_not_null(mean);
    tcc_assert_not_null(variance);
}

expr parse_op_relu6(expr features)
{
    tcc_assert_not_null(features);

    ranges i = to_ranges(features->shape);
    return select::make(i,
                        index::make(i, features, i) > cnst::make(0.0f),
                        index::make(i, features, i),
                        cnst::make(0.0f));
}

expr parse_op_reshape(expr tensor, expr shape)
{
    tcc_assert_not_null(tensor);
    tcc_assert_not_null(shape);
}

expr parse_op_shape(expr input)
{
    tcc_assert_not_null(input);
}

expr parse_op_softmax(expr logits)
{
    tcc_assert_not_null(logits);
}

expr parse_op_squeeze(std::vector<long> sqeeuze_dims, expr input)
{
    tcc_assert_not_null(input);
}

} // namespace frontend
} // namespace tcc
