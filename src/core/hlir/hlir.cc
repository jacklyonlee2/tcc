#include "tcc/core/hlir/hlir.h"

#include "tcc/core/common/logging.h"

namespace tcc {
namespace core {

/* HLIRVisitor method implementations. */

void HLIRVisitor::recurse(Op op) {
    if (visited.find(op) == visited.end()) {
        visited.insert(op);
        op->accept(this);
    }
}

void HLIRVisitor::recurse(OpRef op) {
    CHECK_NOT_EXPIRED(op);
    recurse(op.lock());
}

void HLIRVisitor::visit(const op::PlaceholderPtr) {
}

void HLIRVisitor::visit(const op::ConstantPtr) {
}

void HLIRVisitor::visit(const op::IntermediatePtr op) {
    recurse(op->prev_op);
}

void HLIRVisitor::visit(const op::AddPtr op) {
    recurse(op->x);
    recurse(op->y);
}

void HLIRVisitor::visit(const op::AvgPoolPtr op) {
    recurse(op->value);
}

void HLIRVisitor::visit(const op::BiasAddPtr op) {
    recurse(op->input);
    recurse(op->bias);
}

void HLIRVisitor::visit(const op::Conv2DPtr op) {
    recurse(op->input);
    recurse(op->filter);
}

void HLIRVisitor::visit(const op::DepthwiseConv2dNativePtr op) {
    recurse(op->input);
    recurse(op->filter);
}

void HLIRVisitor::visit(const op::FusedBatchNormPtr op) {
    recurse(op->x);
    recurse(op->scale);
    recurse(op->offset);
    recurse(op->mean);
    recurse(op->variance);
}

void HLIRVisitor::visit(const op::Relu6Ptr op) {
    recurse(op->features);
}

void HLIRVisitor::visit(const op::ReshapePtr op) {
    recurse(op->tensor);
    recurse(op->shape);
}

void HLIRVisitor::visit(const op::ShapePtr op) {
    recurse(op->input);
}

void HLIRVisitor::visit(const op::SoftmaxPtr op) {
    recurse(op->logits);
}

void HLIRVisitor::visit(const op::SqueezePtr op) {
    recurse(op->input);
}

/* HLIR method implementations. */

void HLIR::accept(HLIRVisitor *v) const {
    for (Op op : ops) {
        v->recurse(op);
    }
}

} // namespace core
} // namespace tcc
