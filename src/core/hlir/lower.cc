#include "tcc/core/hlir/lower.h"

#include <math.h>

#include "tcc/core/common/util.h"

namespace tcc {
namespace core {

LLIR HLIRLowerer::lower() {
    return LLIR({});
}

Expr HLIRLowerer::get_expr(Op op) const {
    CHECK_NOTNULL(op);
    CHECK_KEY_IN_MAP(op, lowered_op_map) <<
        "Requested Op is not lowered yet.";

    Expr expr = lowered_op_map.at(op);
    CHECK_NOTNULL(expr);
    return expr;
}

void HLIRLowerer::set_expr(Op op, Expr expr) {
    CHECK_NOTNULL(op);
    CHECK_NOTNULL(expr);
    CHECK_KEY_NOT_IN_MAP(op, lowered_op_map) <<
        "Lowered Op is already assigned.";

    lowered_op_map.insert({op, expr});
}

/* Overloaded HLIR Op visitors. */

void HLIRLowerer::visit(const op::PlaceholderPtr op) {
    Expr expr = expr::Var::make(op->data_desc);
    set_expr(op, expr);
}

void HLIRLowerer::visit(const op::ConstantPtr op) {
    Expr expr = expr::Const::make(op->data);
    set_expr(op, expr);
}

void HLIRLowerer::visit(const op::AddPtr op) {
    recurse(op->x);
    recurse(op->y);

    Expr x = get_expr(op->x);
    Expr y = get_expr(op->y);

    std::vector<long> x_shape = x->data_desc.get_shape();
    std::vector<long> y_shape = y->data_desc.get_shape();

    CHECK(x_shape == y_shape) <<
        "x shape: " << x_shape << " y shape: " << y_shape;

    Expr z = expr::Add::make(x, y);

    set_expr(op->z, z);
}

void HLIRLowerer::visit(const op::AvgPoolPtr op) {
    recurse(op->value);

    CHECK(op->ksize.size() == 4 &&
            op->ksize[0] == 1 && op->ksize[3] == 1) <<
        "AvgPool only supports pooling along H and W.";
    CHECK(op->strides.size() == 4 &&
            op->strides[0] == 1 && op->strides[3] == 1) <<
        "AvgPool only supports strides along H and W.";
    CHECK(op->data_format == "NHWC") <<
        "AvgPool only supports NHWC format.";
    CHECK(op->padding == "VALID") <<
        "AvgPool only supports VALID padding";

    Expr value = get_expr(op->value);

    std::vector<long> value_shape = value->data_desc.get_shape();

    CHECK(value_shape.size() == 4);

    long i_n, i_h, i_w, i_c;
    i_n = value_shape[0];
    i_h = value_shape[1];
    i_w = value_shape[2];
    i_c = value_shape[3];

    long k_h, k_w, str_h, str_w;
    k_h = op->ksize[1];
    k_w = op->ksize[2];
    str_h = op->strides[1];
    str_w = op->strides[2];

    long o_h, o_w;
    o_h = (i_h-k_h)/str_h+1;
    o_w = (i_w-k_w)/str_w+1;

    Expr input_frag = compute({i_n, o_h, o_w, k_h, k_w, i_c}, [&](Axes i) -> Expr {
            return expr::Select::make(
                    expr::all(
                        i[1]*str_h+i[3] >= 0,
                        i[1]*str_h+i[3] < i_h,
                        i[2]*str_w+i[4] >= 0,
                        i[2]*str_w+i[4] < i_w),
                    expr::index(
                        value,
                        i[0],
                        i[1]*str_h+i[3],
                        i[2]*str_w+i[4],
                        i[5]),
                    expr::Const::make(0.0f));
            });

    Expr r_k_h = expr::Range::make(0, k_h);
    Expr r_k_w = expr::Range::make(0, k_w);

    Expr output = compute({i_n, o_h, o_w, i_c}, [&](Axes i) -> Expr {
            return expr::Reduce::make(
                    ReduceType::AVG, {r_k_h, r_k_w},
                    expr::index(input_frag, i[0], i[1], i[2], r_k_h, r_k_w, i[3]));
            });

    set_expr(op->output, output);
}

void HLIRLowerer::visit(const op::BiasAddPtr op) {
    recurse(op->input);
    recurse(op->bias);

    CHECK(op->data_format == "NHWC") <<
        "BiasAdd only supports NHWC format.";

    Expr input = get_expr(op->input);
    Expr bias = get_expr(op->bias);

    std::vector<long> input_shape = input->data_desc.get_shape();
    std::vector<long> bias_shape = bias->data_desc.get_shape();

    CHECK(input_shape.size() == 4);
    CHECK(bias_shape.size() == 1);
    CHECK(input_shape[3] == bias_shape[0]);

    Expr output = compute(input_shape, [&](Axes i) -> Expr {
            return expr::Add::make(
                    expr::index(
                        input, i[0], i[1], i[2], i[3]),
                    expr::index(
                        bias, i[3]));
            });

    set_expr(op->output, output);
}

void HLIRLowerer::visit(const op::Conv2DPtr op) {
    recurse(op->input);
    recurse(op->filter);

    CHECK(op->dilations == std::vector<long>({1, 1, 1, 1})) <<
        "Conv2D does not support dilation.";
    CHECK(op->strides.size() == 4 &&
            op->strides[0] == 1 && op->strides[3] == 1) <<
        "Conv2D only supports strides along H and W.";
    CHECK(op->data_format == "NHWC") <<
        "Conv2D only supports NHWC format.";
    CHECK(op->padding == "SAME") <<
        "Conv2D only supports SAME padding";

    Expr input = get_expr(op->input);
    Expr filter = get_expr(op->filter);

    std::vector<long> input_shape = input->data_desc.get_shape();
    std::vector<long> filter_shape = filter->data_desc.get_shape();

    CHECK(input_shape.size() == 4);
    CHECK(filter_shape.size() == 4);

    long i_n, i_h, i_w, i_c, f_h, f_w, f_c, f_n;
    i_n = input_shape[0];
    i_h = input_shape[1];
    i_w = input_shape[2];
    i_c = input_shape[3];
    f_h = filter_shape[0];
    f_w = filter_shape[1];
    f_c = filter_shape[2];
    f_n = filter_shape[3];

    CHECK(f_h % 2 == 1 && f_w % 2 == 1) <<
        "Filter shape: " << filter_shape;

    long str_h, str_w, pad_h, pad_w;
    pad_h = f_h / 2;
    pad_w = f_w / 2;
    str_h = op->strides[1];
    str_w = op->strides[2];

    CHECK(i_n == 1 && i_c == f_c && f_h <= i_h && f_w <= i_w) <<
        "Input shape: " << input_shape << " Filter shape: " << filter_shape;

    long o_n, o_h, o_w, o_c;
    o_n = i_n;
    o_h = floor(static_cast<double>(i_h+pad_h*2-f_h)/str_h + 1);
    o_w = floor(static_cast<double>(i_w+pad_w*2-f_w)/str_w + 1);
    o_c = f_n;

    Expr input_frag = compute({o_n, o_h, o_w, f_h, f_w, f_c}, [&](Axes i) -> Expr {
            return expr::Select::make(
                    expr::all(
                        i[1]*str_h+i[3]-pad_h*2 >= 0,
                        i[1]*str_h+i[3]-pad_h*2 < i_h,
                        i[2]*str_w+i[4]-pad_w*2 >= 0,
                        i[2]*str_w+i[4]-pad_w*2 < i_w),
                    expr::index(
                        input,
                        i[0],
                        i[1]*str_h+i[3]-pad_h*2,
                        i[2]*str_w+i[4]-pad_w*2,
                        i[5]),
                    expr::Const::make(0.0f));
            });

    Expr product = compute({o_n, o_h, o_w, o_c, f_h, f_w, f_c}, [&](Axes i) -> Expr {
            return expr::Mul::make(
                    expr::index(input_frag, i[0], i[1], i[2], i[4], i[5], i[6]),
                    expr::index(filter, i[4], i[5], i[6], i[3]));
            });

    Expr r_f_h = expr::Range::make(0, f_h);
    Expr r_f_w = expr::Range::make(0, f_w);
    Expr r_f_c = expr::Range::make(0, f_c);
    Expr reduced = compute({o_n, o_h, o_w, o_c}, [&](Axes i) -> Expr {\
            return expr::Reduce::make(
                    ReduceType::SUM, {r_f_h, r_f_w, r_f_c},
                    expr::index(product, i[0], i[1], i[2], i[3], r_f_h, r_f_w, r_f_c));
            });

    set_expr(op->output, reduced);
}

void HLIRLowerer::visit(const op::DepthwiseConv2dNativePtr op) {
    recurse(op->input);
    recurse(op->filter);

    CHECK(op->dilations == std::vector<long>({1, 1, 1, 1})) <<
        "DepthwiseConv2dNative does not support dilation.";
    CHECK(op->strides.size() == 4 &&
            op->strides[0] == 1 && op->strides[3] == 1) <<
        "DepthwiseConv2dNative only supports strides along H and W.";
    CHECK(op->data_format == "NHWC") <<
        "DepthwiseConv2dNative only supports NHWC format.";
    CHECK(op->padding == "SAME") <<
        "DepthwiseConv2dNative only supports SAME padding";

    Expr input = get_expr(op->input);
    Expr filter = get_expr(op->filter);

    std::vector<long> input_shape = input->data_desc.get_shape();
    std::vector<long> filter_shape = filter->data_desc.get_shape();

    CHECK(input_shape.size() == 4);
    CHECK(filter_shape.size() == 4);

    long i_n, i_h, i_w, i_c, f_h, f_w, f_c, f_n;
    i_n = input_shape[0];
    i_h = input_shape[1];
    i_w = input_shape[2];
    i_c = input_shape[3];
    f_h = filter_shape[0];
    f_w = filter_shape[1];
    f_c = filter_shape[2];
    f_n = filter_shape[3];

    CHECK(f_h % 2 == 1 && f_w % 2 == 1) <<
        "Filter shape: " << filter_shape;

    long str_h, str_w, pad_h, pad_w;
    pad_h = f_h / 2;
    pad_w = f_w / 2;
    str_h = op->strides[1];
    str_w = op->strides[2];

    CHECK(i_n == 1 && i_c == f_c && f_h <= i_h && f_w <= i_w) <<
        "Input shape: " << input_shape << " Filter shape: " << filter_shape;

    long o_n, o_h, o_w, o_c;
    o_n = i_n;
    o_h = floor(static_cast<double>(i_h+pad_h*2l-f_h)/str_h + 1l);
    o_w = floor(static_cast<double>(i_w+pad_w*2l-f_w)/str_w + 1l);
    o_c = f_n * f_c;

    Expr input_frag = compute({o_n, o_h, o_w, f_h, f_w, f_c}, [&](Axes i) -> Expr {
            return expr::Select::make(
                    expr::all(
                        i[1]*str_h+i[3]-pad_h*2 >= 0,
                        i[1]*str_h+i[3]-pad_h*2 < i_h,
                        i[2]*str_w+i[4]-pad_w*2 >= 0,
                        i[2]*str_w+i[4]-pad_w*2 < i_w),
                    expr::index(
                        input,
                        i[0],
                        i[1]*str_h+i[3]-pad_h*2,
                        i[2]*str_w+i[4]-pad_w*2,
                        i[5]),
                    expr::Const::make(0.0f));
            });

    Expr product = compute({o_n, o_h, o_w, o_c, f_h, f_w}, [&](Axes i) -> Expr {
            return expr::Mul::make(
                    expr::index(input_frag, i[0], i[1], i[2], i[4], i[5], i[3] % f_n),
                    expr::index(filter, i[4], i[5], i[3] % f_n, i[3] % f_c));
            });

    Expr r_f_h = expr::Range::make(0, f_h);
    Expr r_f_w = expr::Range::make(0, f_w);
    Expr reduced = compute({o_n, o_h, o_w, o_c}, [&](Axes i) -> Expr {\
            return expr::Reduce::make(
                    ReduceType::SUM, {r_f_h, r_f_w},
                    expr::index(product, i[0], i[1], i[2], i[3], r_f_h, r_f_w));
            });

    set_expr(op->output, reduced);
}

void HLIRLowerer::visit(const op::FusedBatchNormPtr op) {
    recurse(op->x);
    recurse(op->scale);
    recurse(op->offset);
    recurse(op->mean);
    recurse(op->variance);

    CHECK(op->data_format == "NHWC") <<
        "FusedBatchNorm only supports NHWC format.";

    Expr x = get_expr(op->x);
    Expr scale = get_expr(op->scale);
    Expr offset = get_expr(op->offset);
    Expr mean = get_expr(op->mean);
    Expr variance = get_expr(op->variance);

    std::vector<long> x_shape = x->data_desc.get_shape();
    std::vector<long> scale_shape = scale->data_desc.get_shape();
    std::vector<long> offset_shape = offset->data_desc.get_shape();
    std::vector<long> mean_shape = mean->data_desc.get_shape();
    std::vector<long> variance_shape = variance->data_desc.get_shape();

    CHECK(x_shape.size() == 4);
    CHECK(scale_shape.size() == 1 && scale_shape[0] == x_shape[3]);
    CHECK(offset_shape.size() == 1 && offset_shape[0] == x_shape[3]);
    CHECK(mean_shape.size() == 1 && mean_shape[0] == x_shape[3]);
    CHECK(variance_shape.size() == 1 && variance_shape[0] == x_shape[3]);


    Expr norm = compute(x_shape, [&](Axes i) -> Expr {
            return expr::index(x, i[0], i[1], i[2], i[3]) - expr::index(mean, i[3]);
            });

    Expr quotient = compute(x_shape, [&](Axes i) -> Expr {
            Expr dev = expr::Sqrt::make(variance + expr::Const::make(op->epsilon));
            return expr::index(norm, i[0], i[1], i[2], i[3]) / expr::index(dev, i[3]);
            });

    Expr scaled = compute(x_shape, [&](Axes i) -> Expr {
            return expr::index(quotient, i[0], i[1], i[2], i[3]) / expr::index(scale, i[3]);
            });

    Expr y = compute(x_shape, [&](Axes i) -> Expr {
            return expr::index(scaled, i[0], i[1], i[2], i[3]) / expr::index(offset, i[3]);
            });

    set_expr(op->y, y);
}

void HLIRLowerer::visit(const op::Relu6Ptr op) {
    recurse(op->features);

    Expr features = get_expr(op->features);

    std::vector<long> features_shape = features->data_desc.get_shape();

    Expr activations = compute(features_shape, [&](Axes i) -> Expr {
            return expr::Select::make(
                    expr::index(features, i[0], i[1], i[2], i[3]) >
                        expr::Const::make(0.0f),
                    expr::index(features, i[0], i[1], i[2], i[3]),
                    expr::Const::make(0.0f));
            });

    set_expr(op->activations, activations);
}

void HLIRLowerer::visit(const op::ReshapePtr op) {
    recurse(op->tensor);
    recurse(op->shape);

    Expr tensor = get_expr(op->tensor);
    Expr shape = get_expr(op->shape);

    CHECK(shape->expr_type == ExprType::Const) <<
        "Reshape shape must be of type expr::Const.";

    Data data = expr::downcast<expr::Const>(shape)->data;
    std::vector<long> output_shape;

    if (data.get_type() == DataType::INT) {
        std::vector<int> int_shape = data.get_int();
        std::copy(int_shape.begin(), int_shape.end(),
                std::back_inserter(output_shape));

    } else if (data.get_type() == DataType::LONG) {
        output_shape = data.get_long();

    } else LOG(FATAL) << "Supported shape data type.";

    Expr output = expr::reshape(tensor, output_shape);

    set_expr(op->output, output);
}

void HLIRLowerer::visit(const op::ShapePtr op) {
    recurse(op->input);

    Expr input = get_expr(op->input);
    std::vector<long> input_shape = input->data_desc.get_shape();
    Expr output = expr::Const::make(input_shape);

    set_expr(op->output, output);
}

void HLIRLowerer::visit(const op::SoftmaxPtr op) {
    recurse(op->logits);

    Expr logits = get_expr(op->logits);

    std::vector<long> logits_shape = logits->data_desc.get_shape();

    LOG(ERROR) << logits_shape;
}

void HLIRLowerer::visit(const op::SqueezePtr op) {
    recurse(op->input);

    Expr input = get_expr(op->input);

    std::vector<long> input_shape = input->data_desc.get_shape();
    std::vector<long> output_shape;
    for (long dim : input_shape) {
        if (dim != 1) {
            output_shape.push_back(dim);
        }
    }

    Expr output = expr::reshape(input, output_shape);

    set_expr(op->output, output);
}

} // namespace core
} // namespace tcc
