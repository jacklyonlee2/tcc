#include "tcc/frontend/op.h"
#include "tcc/common/logging.h"
#include <math.h>

namespace tcc {
namespace frontend {

affn::expr build_placeholder(data_type dtype, dimensions shape)
{
    return affn::var::make(dtype, shape);
}

affn::expr build_const(std::string data, data_type dtype, dimensions shape)
{
    return affn::cnst::make(data, dtype, shape);
}

affn::expr build_add(affn::expr x, affn::expr y)
{
    return x + y;
}

affn::expr build_avgpool(std::string data_format,
                      std::string padding,
                      dimensions ksize,
                      dimensions strides,
                      affn::expr value)
{
    using namespace tcc::affn;

    tcc_assert(data_format == "NHWC",
               "\"" + data_format + "\" data format is not supported.");
    tcc_assert(padding == "VALID",
               "\"" + padding + "\" padding is not supported.");
    tcc_assert(ksize.size() == 4 && ksize[0] == 1 && ksize[3] == 1,
               "pooling along batch or channel dimension are not supported.");
    tcc_assert(strides.size() == 4 && strides[0] == 1 && strides[3] == 1,
               "strides along batch or channel dimension are not supported.");
    tcc_assert_not_null(value);
    tcc_assert_size_eq(value->shape, 4);

    dimension i_n, i_h, i_w, i_c;
    i_n = value->shape[0];
    i_h = value->shape[1];
    i_w = value->shape[2];
    i_c = value->shape[3];

    dimension k_h, k_w, s_h, s_w;
    k_h = ksize[1];
    k_w = ksize[2];
    s_h = strides[1];
    s_w = strides[2];

    dimension o_h, o_w;
    o_h = (i_h - k_h) / s_h + 1;
    o_w = (i_w - k_w) / s_w + 1;

    exprs i = to_ranges({ i_n, o_h, o_w, k_h, k_h, i_c });
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

    return reduce::make(reduce::type::avg, { 3, 4 }, value_frag);
}

affn::expr build_biasadd(std::string data_format, affn::expr input, affn::expr bias)
{
    using namespace tcc::affn;

    tcc_assert(data_format == "NHWC",
               "\"" + data_format + "\" data format is not supported.");
    tcc_assert_not_null(input);
    tcc_assert_not_null(bias);
    tcc_assert_size_eq(input->shape, 4);
    tcc_assert_size_eq(bias->shape, 1);
    tcc_assert(input->shape[3] == bias->shape[0],
               "input channel dimension and bias dimension do not agree.");

    return input + bias;
}

affn::expr build_conv2d(std::string data_format,
                     std::string padding,
                     dimensions strides,
                     dimensions dilations,
                     affn::expr input,
                     affn::expr filter)
{
    using namespace tcc::affn;

    tcc_assert(data_format == "NHWC",
               "\"" + data_format + "\" data format is not supported.");
    tcc_assert(padding == "SAME",
               "\"" + padding + "\" padding is not supported.");
    tcc_assert(strides.size() == 4 && strides[0] == 1 && strides[3] == 1,
               "strides along batch or channel dimension are not supported.");
    tcc_assert(dilations == dimensions({ 1, 1, 1, 1 }),
               "dilations are not supported.");
    tcc_assert_not_null(input);
    tcc_assert_not_null(filter);
    tcc_assert_size_eq(input->shape, 4);
    tcc_assert_size_eq(filter->shape, 4);

    dimension i_n, i_h, i_w, i_c;
    i_n = input->shape[0];
    i_h = input->shape[1];
    i_w = input->shape[2];
    i_c = input->shape[3];

    dimension f_h, f_w, f_c, f_n;
    f_h = filter->shape[0];
    f_w = filter->shape[1];
    f_c = filter->shape[2];
    f_n = filter->shape[3];

    tcc_assert(f_h % 2 == 1 && f_w % 2 == 1, "filter dimensions are not even.");

    dimension s_h, s_w, p_h, p_w;
    s_h = strides[1];
    s_w = strides[2];
    p_h = f_h / 2;
    p_w = f_w / 2;

    tcc_assert(i_n == 1, "batched input is not supported.");
    tcc_assert(f_h <= i_h && f_w <= i_w, "invalid filter size.");
    tcc_assert(i_c == f_c,
               "channel dimensions of input and filter do not agree.");

    dimension o_n, o_h, o_w, o_c;
    o_n = i_n;
    o_h = floor(static_cast<double>(i_h + p_h * 2 - f_h) / s_h + 1);
    o_w = floor(static_cast<double>(i_w + p_w * 2 - f_w) / s_w + 1);
    o_c = f_n;

    exprs i = to_ranges({ o_n, o_h, o_w, f_h, f_w, f_c, o_c });
    expr input_frag = select::make(
        i,
        ((i[1] * cnst::make(s_h) + i[3] - cnst::make(p_h) * cnst::make(2l) >=
          cnst::make(0l)) &&
         (i[1] * cnst::make(s_h) + i[3] - cnst::make(p_h) * cnst::make(2l) <
          cnst::make(i_h)) &&
         (i[2] * cnst::make(s_w) + i[4] - cnst::make(p_w) * cnst::make(2l) >=
          cnst::make(0l)) &&
         (i[2] * cnst::make(s_w) + i[4] - cnst::make(p_w) * cnst::make(2l) <
          cnst::make(i_w))),
        index::make(
            i,
            input,
            { i[0],
              i[1] * cnst::make(s_h) + i[3] - cnst::make(p_h) * cnst::make(2l),
              i[2] * cnst::make(s_w) + i[4] - cnst::make(p_w) * cnst::make(2l),
              i[5] }),
        cnst::make(0.0f));

    return reduce::make(reduce::type::sum, { 3, 4, 5 }, input_frag * filter);
}

affn::expr build_depthwiseconv2dnative(std::string data_format,
                                    std::string padding,
                                    dimensions strides,
                                    dimensions dilations,
                                    affn::expr input,
                                    affn::expr filter)
{
    using namespace tcc::affn;

    tcc_assert(data_format == "NHWC",
               "\"" + data_format + "\" data format is not supported.");
    tcc_assert(padding == "SAME",
               "\"" + padding + "\" padding is not supported.");
    tcc_assert(strides.size() == 4 && strides[0] == 1 && strides[3] == 1,
               "strides along batch or channel dimension are not supported.");
    tcc_assert(dilations == dimensions({ 1, 1, 1, 1 }),
               "dilations are not supported.");
    tcc_assert_not_null(input);
    tcc_assert_not_null(filter);
    tcc_assert_size_eq(input->shape, 4);
    tcc_assert_size_eq(filter->shape, 4);

    dimension i_n, i_h, i_w, i_c;
    i_n = input->shape[0];
    i_h = input->shape[1];
    i_w = input->shape[2];
    i_c = input->shape[3];

    dimension f_h, f_w, f_c, f_n;
    f_h = filter->shape[0];
    f_w = filter->shape[1];
    f_c = filter->shape[2];
    f_n = filter->shape[3];

    tcc_assert(f_h % 2 == 1 && f_w % 2 == 1, "filter dimensions are not even.");

    dimension s_h, s_w, p_h, p_w;
    s_h = strides[1];
    s_w = strides[2];
    p_h = f_h / 2;
    p_w = f_w / 2;

    tcc_assert(i_n == 1, "batched input is not supported.");
    tcc_assert(f_h <= i_h && f_w <= i_w, "invalid filter size.");
    tcc_assert(i_c == f_c,
               "channel dimensions of input and filter do not agree.");

    dimension o_n, o_h, o_w, o_c;
    o_n = i_n;
    o_h = floor(static_cast<double>(i_h + p_h * 2 - f_h) / s_h + 1);
    o_w = floor(static_cast<double>(i_w + p_w * 2 - f_w) / s_w + 1);
    o_c = f_n * f_c;

    exprs i = to_ranges({ o_n, o_h, o_w, f_h, f_w, f_c, f_n });
    expr input_frag = select::make(
        i,
        ((i[1] * cnst::make(s_h) + i[3] - cnst::make(p_h) * cnst::make(2l) >=
          cnst::make(0l)) &&
         (i[1] * cnst::make(s_h) + i[3] - cnst::make(p_h) * cnst::make(2l) <
          cnst::make(i_h)) &&
         (i[2] * cnst::make(s_w) + i[4] - cnst::make(p_w) * cnst::make(2l) >=
          cnst::make(0l)) &&
         (i[2] * cnst::make(s_w) + i[4] - cnst::make(p_w) * cnst::make(2l) <
          cnst::make(i_w))),
        index::make(
            i,
            input,
            { i[0],
              i[1] * cnst::make(s_h) + i[3] - cnst::make(p_h) * cnst::make(2l),
              i[2] * cnst::make(s_w) + i[4] - cnst::make(p_w) * cnst::make(2l),
              i[5] }),
        cnst::make(0.0f));

    return reshape::make(
        { o_n, o_h, o_w, o_c },
        reduce::make(reduce::type::sum, { 3, 4 }, input_frag * filter));
}

affn::expr build_fusedbatchnorm(float epsilon,
                             std::string data_format,
                             affn::expr x,
                             affn::expr scale,
                             affn::expr offset,
                             affn::expr mean,
                             affn::expr variance)
{
    using namespace tcc::affn;

    tcc_assert(data_format == "NHWC",
               "\"" + data_format + "\" data format is not supported.");
    tcc_assert_not_null(x);
    tcc_assert_not_null(scale);
    tcc_assert_not_null(offset);
    tcc_assert_not_null(mean);
    tcc_assert_not_null(variance);
    tcc_assert_size_eq(x->shape, 4);
    tcc_assert(scale->shape.size() == 1 && scale->shape[0] == x->shape[3],
               "dimension of scale do not agree with channel dimension of x.");
    tcc_assert(offset->shape.size() == 1 && offset->shape[0] == x->shape[3],
               "dimension of offset do not agree with channel dimension of x.");
    tcc_assert(mean->shape.size() == 1 && mean->shape[0] == x->shape[3],
               "dimension of mean do not agree with channel dimension of x.");
    tcc_assert(
        variance->shape.size() == 1 && variance->shape[0] == x->shape[3],
        "dimension of variance do not agree with channel dimension of x.");

    return ((x - mean) / sqrt::make(variance + cnst::make(epsilon))) * scale +
           offset;
}

affn::expr build_relu6(affn::expr features)
{
    using namespace tcc::affn;

    tcc_assert_not_null(features);

    exprs i = to_ranges(features->shape);
    return select::make(i,
                        index::make(i, features, i) > cnst::make(0.0f),
                        index::make(i, features, i),
                        cnst::make(0.0f));
}

affn::expr build_reshape(affn::expr tensor, affn::expr shape)
{
    using namespace tcc::affn;

    tcc_assert_not_null(tensor);
    tcc_assert_not_null(shape);
    tcc_assert(shape->type == expr_type::cnst,
               "expr type of shape is not cnst.");

    dimensions to_shape;
    if (shape->dtype == data_type::INT64)
    {
        for (dimension dim : downcast<cnst>(shape)->to_vector<dimension>())
        {
            to_shape.push_back(dim < 0 ? 1 : dim);
        }
    }
    else if (shape->dtype == data_type::INT32)
    {
        for (int32_t dim : downcast<cnst>(shape)->to_vector<int32_t>())
        {
            to_shape.push_back(dim < 0 ? 1l : static_cast<dimension>(dim));
        }
    }
    else
    {
        tcc_error("dtype of shape is unsupported.");
    }

    return reshape::make(to_shape, tensor);
}

affn::expr build_shape(affn::expr input)
{
    tcc_assert_not_null(input);
    return affn::cnst::make(input->shape);
}

affn::expr build_softmax(affn::expr logits)
{
    using namespace tcc::affn;

    tcc_assert_not_null(logits);
    tcc_assert_size_eq(logits->shape, 2);

    exprs i = to_ranges(logits->shape);
    expr mx = reduce::make(reduce::type::max, { 0, 1 }, logits);
    expr de = exp::make(logits - mx);
    expr sm = reduce::make(reduce::type::sum, { 1 }, de);
    return de / index::make(i, sm, { i[0] });
}

affn::expr build_squeeze(dimensions squeeze_dims, affn::expr input)
{
    using namespace tcc::affn;

    tcc_assert(!squeeze_dims.empty(), "squeeze_dims is empty.");
    tcc_assert_not_null(input);

    std::unordered_set<dimension> squeeze_dim_set;
    for (dimension dim : squeeze_dims)
    {
        tcc_assert(dim >= 0l &&
                       dim < static_cast<dimension>(input->shape.size()),
                   "squeeze dim is out of bound.");
        squeeze_dim_set.insert(dim);
    }

    dimensions squeezed_shape;
    for (unsigned dim = 0; dim < input->shape.size(); dim++)
    {
        if (squeeze_dim_set.find(dim) == squeeze_dim_set.end())
        {
            squeezed_shape.push_back(input->shape[dim]);
        }
    }

    return reshape::make(squeezed_shape, input);
}

} // namespace frontend
} // namespace tcc
