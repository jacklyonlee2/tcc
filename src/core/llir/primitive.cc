#include "tcc/core/llir/primitive.h"

#include "tcc/core/llir/llir.h"

namespace tcc {
namespace core {

/* Pmt method implementations.
 * Implicit scalar conversions. */

#define IMPLICIT_SCALAR_CONVERSION(type, tensor_type) \
    Pmt::Pmt(type scalar) : \
        Pmt(pmt::Constant::make( \
                    Tensor::tensor_type(scalar))) {}

IMPLICIT_SCALAR_CONVERSION(float, FLOAT)

#undef IMPLICIT_SCALAR_CONVERSION

/* Override LLIR Primitive accept method. */

#define IMPLEMENT_ACCEPT(type) \
    template<> void Primitive<type>::accept(LLIRPmtVisitor *v) const { \
        v->visit(pmt::downcast<type>(shared_from_this())); \
    }

IMPLEMENT_ACCEPT(pmt::Placeholder)
IMPLEMENT_ACCEPT(pmt::Constant)
IMPLEMENT_ACCEPT(pmt::Multiply)
IMPLEMENT_ACCEPT(pmt::Select)
IMPLEMENT_ACCEPT(pmt::ReduceSum)

#undef IMPLEMENT_ACCEPT

namespace pmt {

Pmt Placeholder::make(TensorDesc tensor_desc_) {
    std::shared_ptr<Placeholder> pmt(new Placeholder);
    pmt->tensor_type = tensor_desc_.get_type();
    pmt->set_axes(tensor_desc_.get_shape());
    return pmt;
}

Pmt Constant::make(Tensor tensor_) {
    std::shared_ptr<Constant> pmt(new Constant);
    pmt->tensor_type = tensor_.get_type();
    pmt->tensor = tensor_;
    pmt->set_axes(tensor_.get_shape());
    return pmt;
}

Pmt Multiply::make(Pmt x_, Pmt y_) {
    CHECK(x_->tensor_type == y_->tensor_type) <<
        "TensorType must agree between x_ and y_.";
    CHECK_AXES_SET(x_);
    CHECK_AXES_SET(y_);

    std::shared_ptr<Multiply> pmt(new Multiply);
    pmt->tensor_type = x_->tensor_type;
    pmt->x = x_;
    pmt->y = y_;
    return pmt;
}

Pmt Select::make(
        Expr condition_,
        Pmt t_,
        Pmt f_) {
    CHECK(condition_->value_type == ValueType::BOOL) <<
        "condition_ must be of type ValueType::BOOL.";
    CHECK(t_->tensor_type == f_->tensor_type) <<
        "TensorType must agree between t_ and f_.";
    CHECK_AXES_SET(t_);
    CHECK_AXES_SET(f_);

    std::shared_ptr<Select> pmt(new Select);
    pmt->tensor_type = t_->tensor_type;
    pmt->t = t_;
    pmt->f = f_;
    return pmt;
}

Pmt ReduceSum::make(
        Axes reduce_axes_,
        Pmt input_) {
    CHECK(!reduce_axes_.empty()) <<
        "reduce_axes_ can not be empty.";
    CHECK_AXES_SET(input_);

    std::shared_ptr<ReduceSum> pmt(new ReduceSum);
    pmt->tensor_type = input_->tensor_type;
    pmt->reduce_axes = reduce_axes_;
    pmt->input = input_;
    return pmt;
}

} // namespace pmt
} // namespace core
} // namespace tcc
