#ifndef TCC_LLIR_PRIMITIVE_H
#define TCC_LLIR_PRIMITIVE_H

#include "tcc/core/common/tensor.h"
#include "tcc/core/llir/expression.h"

namespace tcc {
namespace core {

/* All LLIR Pmt Type. */

enum class PmtType {
    /* Terminal Primitives */
    Placeholder,
    Constant
};

/* Base class for LLIR Primitive. */

struct BasePrimitive {
    BasePrimitive(PmtType pt) :
        pmt_type(pt), tensor_type(TensorType::UNINITIALIZED) {}

    /* Virtual accept method to support visitor pattern. */
    virtual void accept(LLIRVisitor *v) const = 0;

    std::vector<long> get_shape() const {
        return expr::to_shape(tensor_range);
    }

    PmtType pmt_type;
    TensorType tensor_type;
    mutable expr::Ranges tensor_range;
};

typedef std::shared_ptr<const BasePrimitive> Pmt;
typedef std::pair<Pmt, Expr> PmtInput;

/* Templated LLIR Primitive class.
 * LLIR Primitives are used to express Op kernels
 * at the iteration level. */

template<typename T>
struct Primitive :
    public BasePrimitive,
    public std::enable_shared_from_this<Primitive<T>> {
    Primitive() : BasePrimitive(T::_pmt_type) {}

    void accept(LLIRVisitor *v) const override;
};

namespace pmt {

/* Helper function to downcast Pmt. */

template<typename T> std::shared_ptr<const T> downcast(Pmt pmt) {
    if (pmt && pmt->pmt_type == T::_pmt_type) {
        return std::static_pointer_cast<const T>(pmt);
    } else {
        LOG(FATAL) << "Illegal downcast of Pmt.";
        return nullptr;
    }
}

/* --- Declare LLIR Pmts --- */

#define DECLARE_PRIMITIVE(type) \
    struct type; \
    typedef std::shared_ptr<const type> type##Ptr; \
    struct type : public Primitive<type> { \
        static const PmtType _pmt_type = PmtType::type;
#define END_DECLARE };

DECLARE_PRIMITIVE(Placeholder)
    static Pmt make(TensorDesc tensor_desc_);
END_DECLARE // Placeholder

DECLARE_PRIMITIVE(Constant)
    Tensor tensor;

    static Pmt make(Tensor tensor_);
END_DECLARE // Constant

#undef DECLARE_PRIMITIVE
#undef END_DECLARE

} // namespace pmt
} // namespace core
} // namespace tcc

#endif // TCC_LLIR_PRIMITIVE_H
