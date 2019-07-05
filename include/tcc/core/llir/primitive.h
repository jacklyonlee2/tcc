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
    Constant,
    Multiply,
    Select
};

/* Base class for LLIR Primitive. */

struct BasePrimitive {
    BasePrimitive(PmtType pt) :
        pmt_type(pt), tensor_type(TensorType::UNINITIALIZED) {}

    /* Virtual accept method to support visitor pattern. */
    virtual void accept(LLIRVisitor *v) const = 0;

    std::vector<long> get_shape() const {
        CHECK_NOTNULL(axes);
        return axes->to_shape();
    }

    Axes get_axes() const {
        CHECK_NOTNULL(axes);
        return *axes;
    }

    void set_axes(Axes axes_) const {
        CHECK(axes == nullptr) << "Axes are already set.";
        axes = std::make_shared<Axes>(axes_);
    }

    bool axes_set() const { return axes != nullptr; }

    PmtType pmt_type;
    TensorType tensor_type;

    private:
        mutable std::shared_ptr<Axes> axes;
};

/* A wrapper around std::shared_ptr<const BasePrimitive>.
 * Provides overloaded '()' operator to produce accessed Pmt
 * from the given LLIR Expressions.
 * Provides implicit conversion from Pmt. */
struct Pmt : public std::shared_ptr<const BasePrimitive> {
    using shared_ptr<const BasePrimitive>::shared_ptr;

    template <typename ... Args>
    Pmt operator()(Args ... args) const {
        CHECK(access_pattern == nullptr) <<
            "Primitive has already been accessed.";

        std::vector<Expr> exprs = expr::accumulate_parameters(args...);
        CHECK(exprs.size() == (*this)->get_axes().size()) <<
            "Access expressions does not agree with primitive tensor rank.";

        Pmt accessed = Pmt(*this);
        accessed.access_pattern = std::make_shared<std::vector<Expr>>(exprs);
        return accessed;
    }

    private:
        std::shared_ptr<std::vector<Expr>> access_pattern;
};

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

/* --- Declare LLIR Pmts --- */

/* Declare struct of 'type' inheriting from Primitive<type>.
 * Define type alias 'typePtr' referring to
 * shared pointer to const 'type' object.
 * Create static member '_pmt_type' to be used by
 * base class constructor and downcast functions. */
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

DECLARE_PRIMITIVE(Multiply)
    Pmt x;
    Pmt y;

    static Pmt make(Pmt x_, Pmt y_);
END_DECLARE // Multiply

DECLARE_PRIMITIVE(Select)
    Expr condition;

    Pmt t;
    Pmt f;

    static Pmt make(
            Expr condition_,
            Pmt t_,
            Pmt f_);
END_DECLARE // Select

#undef DECLARE_PRIMITIVE
#undef END_DECLARE

/* Downcast function for Pmt. */
template<typename T> std::shared_ptr<const T> downcast(Pmt pmt) {
    if (pmt && pmt->pmt_type == T::_pmt_type) {
        return std::static_pointer_cast<const T>(pmt);
    } else {
        LOG(FATAL) << "Illegal downcast of Pmt.";
        return nullptr;
    }
}

} // namespace pmt
} // namespace core
} // namespace tcc

namespace std {

/* Specializing std::hash for tcc::core::Pmt. */
template<> struct hash<tcc::core::Pmt> {
    size_t operator()(const tcc::core::Pmt& pmt) const {
        return hash<shared_ptr<const tcc::core::BasePrimitive>>()(pmt);
    }
};

} // namespace std

#endif // TCC_LLIR_PRIMITIVE_H
