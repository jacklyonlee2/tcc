#include "tcc/core/hlir/operation.h"

#include "tcc/core/hlir/hlir.h"
#include "tcc/core/common/logging.h"

namespace tcc {
namespace core {

/* Override HLIR Operation accept method. */

#define IMPLEMENT_ACCEPT(type) \
    template<> void Operation<type>::accept(HLIRVisitor *v) const { \
        v->visit(op::downcast<type>(shared_from_this())); \
    }

IMPLEMENT_ACCEPT(op::Placeholder)
IMPLEMENT_ACCEPT(op::Constant)
IMPLEMENT_ACCEPT(op::Intermediate)
IMPLEMENT_ACCEPT(op::Add)
IMPLEMENT_ACCEPT(op::AvgPool)
IMPLEMENT_ACCEPT(op::BiasAdd)
IMPLEMENT_ACCEPT(op::Conv2D)
IMPLEMENT_ACCEPT(op::DepthwiseConv2dNative)
IMPLEMENT_ACCEPT(op::FusedBatchNorm)
IMPLEMENT_ACCEPT(op::Relu6)
IMPLEMENT_ACCEPT(op::Reshape)
IMPLEMENT_ACCEPT(op::Shape)
IMPLEMENT_ACCEPT(op::Softmax)
IMPLEMENT_ACCEPT(op::Squeeze)

#undef IMPLEMENT_ACCEPT

namespace op {

/* --- Implement HLIR Ops --- */

Op Placeholder::make(DataDesc data_desc_) {
    std::shared_ptr<Placeholder> op(new Placeholder);
    op->data_desc = data_desc_;
    return op;
}

Op Constant::make(Data data_) {
    std::shared_ptr<Constant> op(new Constant);
    op->data = data_;
    return op;
}

Op Intermediate::make(Op prev_op_) {
    CHECK_NOTNULL(prev_op_);

    std::shared_ptr<Intermediate> op(new Intermediate);
    op->prev_op = prev_op_;
    return op;
}

Op Add::make(Op x_, Op y_) {
    CHECK_NOTNULL(x_);
    CHECK_NOTNULL(y_);

    std::shared_ptr<Add> op(new Add);
    op->x = x_;
    op->y = y_;
    op->z = Intermediate::make(op);
    return op;
}

Op AvgPool::make(
        std::string data_format_,
        std::string padding_,
        std::vector<int64_t> ksize_,
        std::vector<int64_t> strides_,
        Op value_) {
    CHECK_NOTNULL(value_);

    std::shared_ptr<AvgPool> op(new AvgPool);
    op->data_format = data_format_;
    op->padding = padding_;
    op->ksize = ksize_;
    op->strides = strides_;
    op->value = value_;
    op->output = Intermediate::make(op);
    return op;
}

Op BiasAdd::make(
        std::string data_format_,
        Op input_,
        Op bias_) {
    CHECK_NOTNULL(input_);
    CHECK_NOTNULL(bias_);

    std::shared_ptr<BiasAdd> op(new BiasAdd);
    op->data_format = data_format_;
    op->input = input_;
    op->bias = bias_;
    op->output = Intermediate::make(op);
    return op;
}

Op Conv2D::make(
        std::string data_format_,
        std::string padding_,
        std::vector<int64_t> strides_,
        std::vector<int64_t> dilations_,
        Op input_,
        Op filter_) {
    CHECK_NOTNULL(input_);
    CHECK_NOTNULL(filter_);

    std::shared_ptr<Conv2D> op(new Conv2D);
    op->data_format = data_format_;
    op->padding = padding_;
    op->strides = strides_;
    op->dilations = dilations_;
    op->input = input_;
    op->filter = filter_;
    op->output = Intermediate::make(op);
    return op;
}

Op DepthwiseConv2dNative::make(
        std::string data_format_,
        std::string padding_,
        std::vector<int64_t> strides_,
        std::vector<int64_t> dilations_,
        Op input_,
        Op filter_) {
    CHECK_NOTNULL(input_);
    CHECK_NOTNULL(filter_);

    std::shared_ptr<DepthwiseConv2dNative> op(new DepthwiseConv2dNative);
    op->data_format = data_format_;
    op->padding = padding_;
    op->strides = strides_;
    op->dilations = dilations_;
    op->input = input_;
    op->filter = filter_;
    op->output = Intermediate::make(op);
    return op;
}

Op FusedBatchNorm::make(
        float epsilon_,
        std::string data_format_,
        Op x_,
        Op scale_,
        Op offset_,
        Op mean_,
        Op variance_) {
    CHECK_NOTNULL(x_);
    CHECK_NOTNULL(scale_);
    CHECK_NOTNULL(offset_);
    CHECK_NOTNULL(mean_);
    CHECK_NOTNULL(variance_);

    std::shared_ptr<FusedBatchNorm> op(new FusedBatchNorm);
    op->epsilon = epsilon_;
    op->data_format = data_format_;
    op->x = x_;
    op->scale = scale_;
    op->offset = offset_;
    op->mean = mean_;
    op->variance = variance_;
    op->y = Intermediate::make(op);
    return op;
}

Op Relu6::make(Op features_) {
    CHECK_NOTNULL(features_);

    std::shared_ptr<Relu6> op(new Relu6);
    op->features = features_;
    op->activations = Intermediate::make(op);
    return op;
}

Op Reshape::make(Op tensor_, Op shape_) {
    CHECK_NOTNULL(tensor_);
    CHECK_NOTNULL(shape_);

    std::shared_ptr<Reshape> op(new Reshape);
    op->tensor = tensor_;
    op->shape = shape_;
    op->output = Intermediate::make(op);
    return op;
}

Op Shape::make(Op input_) {
    CHECK_NOTNULL(input_);

    std::shared_ptr<Shape> op(new Shape);
    op->input = input_;
    op->output = Intermediate::make(op);
    return op;
}

Op Softmax::make(Op logits_) {
    CHECK_NOTNULL(logits_);

    std::shared_ptr<Softmax> op(new Softmax);
    op->logits = logits_;
    op->output = Intermediate::make(op);
    return op;
}

Op Squeeze::make(std::vector<int64_t> squeeze_dims_, Op input_) {
    CHECK_NOTNULL(input_);

    std::shared_ptr<Squeeze> op(new Squeeze);
    op->squeeze_dims = squeeze_dims_;
    op->input = input_;
    op->output = Intermediate::make(op);
    return op;
}

} // namespace op
} // namespace core
} // namespace tcc
