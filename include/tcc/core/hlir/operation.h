#ifndef TCC_HLIR_OPERATION_H
#define TCC_HLIR_OPERATION_H

#include <memory>

#include "tcc/core/common/tensor.h"

namespace tcc {
namespace core {

class HLIRVisitor;

/* All HLIR Op types. */

enum class OpType {
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
    BaseOperation(OpType t) : op_type(t) {}

    /* Virtual accept method to support visitor pattern. */
    virtual void accept(HLIRVisitor *v) const = 0;

    OpType op_type;
};

typedef std::shared_ptr<const BaseOperation> Op;
typedef std::weak_ptr<const BaseOperation> OpRef;

/* Templated HLIR Operation class.
 * HLIR Operations are used to represent graph level nn ops.
 * Such as 'Convolution' and 'Maxpool'. */

template<typename T>
struct Operation :
    public BaseOperation,
    public std::enable_shared_from_this<Operation<T>> {
    Operation() : BaseOperation(T::_op_type) {}

    void accept(HLIRVisitor *v) const override;
};

namespace op {

/* --- Declare HLIR Ops --- */

/* Declare struct of 'type' inheriting from Operation<type>.
 * Define type alias 'typePtr' referring to
 * shared pointer to const 'type' object.
 * Create static member '_op_type' to be used by
 * base class constructor and downcast functions. */
#define DECLARE_OPERATION(type) \
    struct type; \
    typedef std::shared_ptr<const type> type##Ptr; \
    struct type : public Operation<type> { \
        static const OpType _op_type = OpType::type;
#define END_DECLARE };

DECLARE_OPERATION(Placeholder)
    TensorDesc tensor_desc;

    static Op make(TensorDesc tensor_desc_);
END_DECLARE // Placeholder

DECLARE_OPERATION(Constant)
    Tensor tensor;

    static Op make(Tensor tensor_);
END_DECLARE // Constant

DECLARE_OPERATION(Intermediate)
    OpRef prev_op;

    static Op make(Op prev_op_);
END_DECLARE // Intermediate

DECLARE_OPERATION(Add)
    Op x;
    Op y;
    Op z;

    static Op make(Op x_, Op y_);
END_DECLARE // Add

DECLARE_OPERATION(AvgPool)
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
END_DECLARE // AvgPool

DECLARE_OPERATION(BiasAdd)
    std::string data_format;

    Op input;
    Op bias;
    Op output;

    static Op make(
            std::string data_format_,
            Op input_,
            Op bias_);
END_DECLARE // BiasAdd

DECLARE_OPERATION(Conv2D)
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
END_DECLARE // Conv2D

DECLARE_OPERATION(DepthwiseConv2dNative)
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
END_DECLARE // DepthwiseConv2dNative

DECLARE_OPERATION(FusedBatchNorm)
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
END_DECLARE // FusedBatchNorm

DECLARE_OPERATION(Relu6)
    Op features;
    Op activations;

    static Op make(Op features_);
END_DECLARE // Relu6

DECLARE_OPERATION(Reshape)
    Op tensor;
    Op shape;
    Op output;

    static Op make(Op tensor_, Op shape_);
END_DECLARE // Reshape

DECLARE_OPERATION(Shape)
    Op input;
    Op output;

    static Op make(Op input_);
END_DECLARE // Shape

DECLARE_OPERATION(Softmax)
    Op logits;
    Op output;

    static Op make(Op logits_);
END_DECLARE // Softmax

DECLARE_OPERATION(Squeeze)
    std::vector<long> squeeze_dims;

    Op input;
    Op output;

    static Op make(std::vector<long> squeeze_dims_, Op input_);
END_DECLARE // Squeeze

#undef DECLARE_OPERATION
#undef END_DECLARE

/* Helper functions for Op. */

template<typename T> std::shared_ptr<const T> downcast(Op op) {
    if (op && op->op_type == T::_op_type) {
        return std::static_pointer_cast<const T>(op);
    } else {
        LOG(FATAL) << "Illegal downcast of Op.";
        return nullptr;
    }
}

} // namespace op
} // namespace core
} // namespace tcc

#endif // TCC_HLIR_OPERATION_H
