#include "tcc/core/llir/primitive.h"

#include "tcc/core/llir/llir.h"

namespace tcc {
namespace core {

/* Override LLIR Primitive accept method. */

#define IMPLEMENT_ACCEPT(type) \
    template<> void Primitive<type>::accept(LLIRVisitor *v) const { \
        v->visit(pmt::downcast<type>(shared_from_this())); \
    }

IMPLEMENT_ACCEPT(pmt::Placeholder)
IMPLEMENT_ACCEPT(pmt::Constant)

#undef IMPLEMENT_ACCEPT

namespace pmt {

Pmt Placeholder::make(TensorDesc tensor_desc_) {
    std::shared_ptr<Placeholder> pmt(new Placeholder);
    pmt->tensor_type = tensor_desc_.get_type();
    pmt->tensor_range = expr::from_shape(tensor_desc_.get_shape());
    return pmt;
}

Pmt Constant::make(Tensor tensor_) {
    std::shared_ptr<Constant> pmt(new Constant);
    pmt->tensor_type = tensor_.get_type();
    pmt->tensor_range = expr::from_shape(tensor_.get_shape());
    pmt->tensor = tensor_;
    return pmt;
}

} // namespace pmt
} // namespace core
} // namespace tcc
