#ifndef TCC_LLIR_PRIMITIVE_H
#define TCC_LLIR_PRIMITIVE_H

#include "tcc/core/llir/expression.h"

namespace tcc {
namespace core {

/* All LLIR Pmt Type. */

enum class PmtType {
};

/* Base class for LLIR Primitive. */

struct BasePrimitive {
    BasePrimitive(PmtType pt, TensorType tt) :
        pmt_type(pt), tensor_type(tt) {}

    /* Virtual accept method to support visitor pattern. */
    virtual void accept(LLIRVisitor *v) const = 0;

    PmtType pmt_type;
    TensorType tensor_type;
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
    Primitive(TensorType tt) : BasePrimitive(T::_pmt_type, tt) {}

    void accept(LLIRVisitor *v) const override;
}

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
    struct type : public Primitive<type>

#undef DECLARE_PRIMITIVE

} // namespace pmt
} // namespace core
} // namespace tcc

#endif // TCC_LLIR_PRIMITIVE_H
