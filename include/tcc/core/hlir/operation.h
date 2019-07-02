#ifndef TCC_HLIR_OPERATION_H
#define TCC_HLIR_OPERATION_H

#include <memory>

#include "tcc/core/common/tensor.h"

namespace tcc {
namespace core {

class HLIRVisitor;

/* All HLIR Operation types. */
enum class OperationType {
    /* Storage Operations */
    Placeholder,
    Constant,
    Intermediate,
    /* Other Operations in alphabetical order */
    Add,
    AvgPool,
    BiasAdd,
    Conv2D,
    DepthwiseConv2dNative,
    FusedBatchNorm,
    Relu6,
    Reshape,
    Shape,
    Softmax,
    Squeeze
};

/* Base class for HLIR Operation. */

struct BaseOperation {
    BaseOperation(OperationType t) : operation_type(t) {}

    /* Virtual accept method to support visitor pattern. */
    virtual void accept(HLIRVisitor *v) const = 0;

    OperationType operation_type;
};

typedef std::shared_ptr<const BaseOperation> Op;
typedef std::weak_ptr<const BaseOperation> OpRef;

/* Template class for HLIR Operation. */

template<typename T>
struct Operation :
    public BaseOperation,
    public std::enable_shared_from_this<Operation<T>> {
    Operation() : BaseOperation(T::_operation_type) {}
    void accept(HLIRVisitor *v) const override;
};

namespace op {

/* Helper function to downcast Op. */

template<typename T> std::shared_ptr<const T> downcast(Op op) {
    if (op && op->operation_type == T::_operation_type) {
        return std::static_pointer_cast<const T>(op);
    } else {
        LOG(FATAL) << "Illegal downcast of Op.";
        return nullptr;
    }
}

/* --- Declare HLIR Ops --- */

#define DECLARE_OP(type) \
    struct type; \
    typedef std::shared_ptr<const type> type##Ptr; \
    struct type : public Operation<type>

DECLARE_OP(Placeholder) {
    TensorDesc tensor_desc;

    static Op make(TensorDesc tensor_desc_);

    static const OperationType _operation_type = OperationType::Placeholder;
};

DECLARE_OP(Constant) {
    Tensor tensor;

    static Op make(Tensor tensor_);

    static const OperationType _operation_type = OperationType::Constant;
};

DECLARE_OP(Intermediate) {
    OpRef prev_op;

    static Op make(Op prev_op_);

    static const OperationType _operation_type = OperationType::Intermediate;
};

DECLARE_OP(Add) {
    Op x;
    Op y;
    Op z;

    static Op make(Op x_, Op y_);

    static const OperationType _operation_type = OperationType::Add;
};

DECLARE_OP(AvgPool) {
    std::string data_format;
    std::string padding;
    std::vector<long> ksize;
    std::vector<long> strides;

    Op value;
    Op output;

    static Op make(
            std::string data_format_,
            std::string padding_,
            std::vector<long> ksize_,
            std::vector<long> strides_,
            Op value_);

    static const OperationType _operation_type = OperationType::AvgPool;
};

DECLARE_OP(BiasAdd) {
    std::string data_format;

    Op input;
    Op bias;
    Op output;

    static Op make(
            std::string data_format_,
            Op input_,
            Op bias_);

    static const OperationType _operation_type = OperationType::BiasAdd;
};

DECLARE_OP(Conv2D) {
    std::string data_format;
    std::string padding;
    std::vector<long> strides;
    std::vector<long> dilations;

    Op input;
    Op filter;
    Op output;

    static Op make(
            std::string data_format_,
            std::string padding_,
            std::vector<long> strides_,
            std::vector<long> dilations_,
            Op input_,
            Op filter_);

    static const OperationType _operation_type = OperationType::Conv2D;
};

DECLARE_OP(DepthwiseConv2dNative) {
    std::string data_format;
    std::string padding;
    std::vector<long> strides;
    std::vector<long> dilations;

    Op input;
    Op filter;
    Op output;

    static Op make(
            std::string data_format_,
            std::string padding_,
            std::vector<long> strides_,
            std::vector<long> dilations_,
            Op input_,
            Op filter_);

    static const OperationType _operation_type = OperationType::DepthwiseConv2dNative;
};

DECLARE_OP(FusedBatchNorm) {
    float epsilon;
    std::string data_format;

    Op x;
    Op scale;
    Op offset;
    Op mean;
    Op variance;
    Op y;

    static Op make(
            float epsilon_,
            std::string data_format_,
            Op x_,
            Op scale_,
            Op offset_,
            Op mean_,
            Op variance_);

    static const OperationType _operation_type = OperationType::FusedBatchNorm;
};

DECLARE_OP(Relu6) {
    Op features;
    Op activations;

    static Op make(Op features_);

    static const OperationType _operation_type = OperationType::Relu6;
};

DECLARE_OP(Reshape) {
    Op tensor;
    Op shape;
    Op output;

    static Op make(Op tensor_, Op shape_);

    static const OperationType _operation_type = OperationType::Reshape;
};

DECLARE_OP(Shape) {
    Op input;
    Op output;

    static Op make(Op input_);

    static const OperationType _operation_type = OperationType::Shape;
};

DECLARE_OP(Softmax) {
    Op logits;
    Op output;

    static Op make(Op logits_);

    static const OperationType _operation_type = OperationType::Softmax;
};

DECLARE_OP(Squeeze) {
    std::vector<long> squeeze_dims;

    Op input;
    Op output;

    static Op make(std::vector<long> squeeze_dims_, Op input_);

    static const OperationType _operation_type = OperationType::Squeeze;
};

#undef DECLARE_OP

} // namespace op
} // namespace core
} // namespace tcc

#endif // TCC_HLIR_OPERATION_H
