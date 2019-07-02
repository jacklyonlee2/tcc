#include "tcc/core/hlir/lower.h"

namespace tcc {
namespace core {

LLIR HLIRLowerer::lower() {
    return LLIR({});
}

/* Overloaded HLIR Op visitors. */

void HLIRLowerer::visit(const op::PlaceholderPtr) {
}

void HLIRLowerer::visit(const op::ConstantPtr) {
}

void HLIRLowerer::visit(const op::IntermediatePtr op) {
    recurse(op->prev_op);
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
