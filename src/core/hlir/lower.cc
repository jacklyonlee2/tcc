#include "tcc/core/hlir/lower.h"

#include <math.h>

#include "tcc/core/common/util.h"

namespace tcc {
namespace core {

LLIR HLIRLowerer::lower() {
    return LLIR({});
}

Pmt HLIRLowerer::get_pmt(Op op) const {
    CHECK_NOTNULL(op);
    CHECK_KEY_IN_MAP(op, lowered_op_map) <<
        "Requested Op is not lowered yet.";

    Pmt pmt = lowered_op_map.at(op);
    CHECK_NOTNULL(pmt);
    return pmt;
}

void HLIRLowerer::set_pmt(Op op, Pmt pmt) {
    CHECK_NOTNULL(op);
    CHECK_NOTNULL(pmt);
    CHECK_KEY_NOT_IN_MAP(op, lowered_op_map) <<
        "Lowered Op is already assigned.";

    lowered_op_map.insert({op, pmt});
}

/* Overloaded HLIR Op visitors. */

void HLIRLowerer::visit(const op::PlaceholderPtr op) {
    Pmt pmt = pmt::Placeholder::make(op->tensor_desc);
    set_pmt(op, pmt);
}

void HLIRLowerer::visit(const op::ConstantPtr op) {
    Pmt pmt = pmt::Constant::make(op->tensor);
    set_pmt(op, pmt);
}

void HLIRLowerer::visit(const op::AddPtr op) {
    recurse(op->x);
    recurse(op->y);
}

void HLIRLowerer::visit(const op::AvgPoolPtr op) {
    recurse(op->value);
}

void HLIRLowerer::visit(const op::BiasAddPtr op) {
    recurse(op->input);
    recurse(op->bias);
}

void HLIRLowerer::visit(const op::Conv2DPtr op) {
    recurse(op->input);
    recurse(op->filter);

    CHECK(op->dilations == vec(1l, 1l, 1l, 1l)) <<
        "Conv2D does not support dilation.";
    CHECK(op->strides.size() == 4 &&
            op->strides[0] == 1 && op->strides[3] == 1) <<
        "Conv2D only supports op->strides along H and W.";
    CHECK(op->data_format == "NHWC") <<
        "Conv2D only supports NHWC format.";
    CHECK(op->padding == "SAME") <<
        "Conv2D only supports SAME padding";

    Pmt input = get_pmt(op->input);
    Pmt filter = get_pmt(op->filter);

    std::vector<long> input_shape = input->tensor_range.to_shape();
    std::vector<long> filter_shape = filter->tensor_range.to_shape();

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

    CHECK(f_h % 2 == 1 && f_w % 2 == 1);

    long str_h, str_w, pad_h, pad_w;
    pad_h = f_h / 2;
    pad_w = f_w / 2;
    str_h = op->strides[1];
    str_w = op->strides[2];

    CHECK(i_n == 1 && i_c == f_c && f_h <= i_h && f_w <= i_w);

    long o_n, o_h, o_w, o_c;
    o_n = i_n;
    o_h = floor(static_cast<double>(i_h+pad_h*2-f_h)/str_h + 1);
    o_w = floor(static_cast<double>(i_w+pad_w*2-f_w)/str_w + 1);
    o_c = f_n;

}

void HLIRLowerer::visit(const op::DepthwiseConv2dNativePtr op) {
    recurse(op->input);
    recurse(op->filter);
}

void HLIRLowerer::visit(const op::FusedBatchNormPtr op) {
    recurse(op->x);
    recurse(op->scale);
    recurse(op->offset);
    recurse(op->mean);
    recurse(op->variance);
}

void HLIRLowerer::visit(const op::Relu6Ptr op) {
    recurse(op->features);
}

void HLIRLowerer::visit(const op::ReshapePtr op) {
    recurse(op->tensor);
    recurse(op->shape);
}

void HLIRLowerer::visit(const op::ShapePtr op) {
    recurse(op->input);
}

void HLIRLowerer::visit(const op::SoftmaxPtr op) {
    recurse(op->logits);
}

void HLIRLowerer::visit(const op::SqueezePtr op) {
    recurse(op->input);
}

} // namespace core
} // namespace tcc
