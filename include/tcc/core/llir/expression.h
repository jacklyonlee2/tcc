#ifndef TCC_LLIR_EXPRESSION_H
#define TCC_LLIR_EXPRESSION_H

#include <memory>

#include "tcc/core/common/logging.h"

namespace tcc {
namespace core {

class LLIRVisitor;

/* All LLIR Expr value types.
 * Denotes the datatype the Expr evaluates to. */

enum class ValueType {
    UNINITIALIZED,
    INDICES,
    BOOL,
    LONG
};

/* All LLIR Expr types. */

enum class ExprType {
    Scalar,
    Range,
    Add,
    Sub,
    Mul,
    Div,
    GreaterEqual,
    Less,
    And
};

/* Base class for LLIR Expression. */

struct BaseExpression {
    BaseExpression(ExprType et) :
        expr_type(et), value_type(ValueType::UNINITIALIZED) {}

    /* Virtual accept method to support visitor pattern. */
    virtual void accept(LLIRVisitor *v) const = 0;

    ExprType expr_type;
    ValueType value_type;
};

/* A wrapper around std::shared_ptr<const BaseExpression>.
 * Provides overloaded arithmetic and logic operators. */

#define OVERLOAD_OPERATOR(symbol) \
    Expr operator symbol (Expr); \
    Expr operator symbol (long);

struct Expr : public std::shared_ptr<const BaseExpression> {
    using shared_ptr<const BaseExpression>::shared_ptr;

    OVERLOAD_OPERATOR(+)
    OVERLOAD_OPERATOR(-)
    OVERLOAD_OPERATOR(*)
    OVERLOAD_OPERATOR(/)
    OVERLOAD_OPERATOR(>=)
    OVERLOAD_OPERATOR(<)
};

#undef OVERLOAD_OPERATOR

/* Templated LLIR Expression class.
 * LLIR Expressions are used to express the access pattern
 * and conditions of LLIR Primitive's inputs.
 * LLIR Expressions are store inside LLIR Primitive
 * along side of the Primitive inputs. */

template<typename T>
struct Expression :
    public BaseExpression,
    public std::enable_shared_from_this<Expression<T>> {
    Expression() : BaseExpression(T::_expr_type) {}

    void accept(LLIRVisitor *v) const override;
};

namespace expr {

/* --- Declare LLIR Exprs --- */

/* Declare struct of 'type' inheriting from Expression<type>.
 * Define type alias 'typePtr' referring to
 * shared pointer to const 'type' object.
 * Create static member '_expr_type' to be used by
 * base class constructor and downcast functions. */
#define DECLARE_EXPRESSION(type) \
    struct type; \
    typedef std::shared_ptr<const type> type##Ptr; \
    struct type : public Expression<type> { \
        static const ExprType _expr_type = ExprType::type;
#define END_DECLARE };

DECLARE_EXPRESSION(Scalar)
    long long_value;

    static Expr make(long value_);
END_DECLARE // Scalar

DECLARE_EXPRESSION(Range)
    std::pair<long, long> range;

    static Expr make(long begin_, long end_);
END_DECLARE // Range

DECLARE_EXPRESSION(Add)
    Expr x;
    Expr y;

    static Expr make(Expr x_, Expr y_);
END_DECLARE // Add

DECLARE_EXPRESSION(Sub)
    Expr x;
    Expr y;

    static Expr make(Expr x_, Expr y_);
END_DECLARE // Sub

DECLARE_EXPRESSION(Mul)
    Expr x;
    Expr y;

    static Expr make(Expr x_, Expr y_);
END_DECLARE // Mul

DECLARE_EXPRESSION(Div)
    Expr x;
    Expr y;

    static Expr make(Expr x_, Expr y_);
END_DECLARE // Div

DECLARE_EXPRESSION(GreaterEqual)
    Expr x;
    Expr y;

    static Expr make(Expr x_, Expr y_);
END_DECLARE // GreaterEqual

DECLARE_EXPRESSION(Less)
    Expr x;
    Expr y;

    static Expr make(Expr x_, Expr y_);
END_DECLARE // Less

DECLARE_EXPRESSION(And)
    Expr x;
    Expr y;

    static Expr make(Expr x_, Expr y_);
END_DECLARE // And

/* Downcast function for Expr. */
template<typename T> std::shared_ptr<const T> downcast(Expr expr) {
    if (expr && expr->expr_type == T::_expr_type) {
        return std::static_pointer_cast<const T>(expr);
    } else {
        LOG(FATAL) << "Illegal downcast of Expr.";
        return nullptr;
    }
}

/* Helper function for grouping logic LLIR Expressions. */
template<typename T> T all(T expr) { return expr; }
template<typename T, typename ... Args> Expr all(T expr, Args ... args) {
    return And::make(expr, all(args...));
}

/* Function to accumulate parameter pack of Expr. */
template<typename T> std::vector<T> accumulate_parameters(T expr) { return {expr}; }
template<typename T, typename ... Args>
std::vector<T> accumulate_parameters(T expr, Args ... args) {
    std::vector<T> exprs({expr});
    std::vector<T> rest = accumulate_parameters(args...);
    exprs.insert(exprs.end(), rest.begin(), rest.end());
    return exprs;
}

} // namespace expr

/* Axes class are store in each LLIR Primitives to
 * denote output shape information; also it serves
 * as input to LLIR compute lambdas (see llir.h). */

struct Axes : public std::vector<Expr> {
    using vector<Expr>::vector;

    Axes(std::vector<long> shape) {
        for (long dim : shape) {
            this->push_back(expr::Range::make(0l, dim));
        }
    }

    std::vector<long> to_shape() const {
        std::vector<long> shape;
        for (Expr expr : *this) {
            expr::RangePtr range = expr::downcast<expr::Range>(expr);
            long dim = range->range.second - range->range.first;
            CHECK(dim > 0) << "Dimension must be positive.";
            shape.push_back(dim);
        }
        return shape;
    }
};

} // namespace core
} // namespace tcc

namespace std {

/* Specializing std::hash for tcc::core::Expr. */
template<> struct hash<tcc::core::Expr> {
    size_t operator()(const tcc::core::Expr& expr) const {
        return hash<shared_ptr<const tcc::core::BaseExpression>>()(expr);
    }
};

} // namespace std

#endif // TCC_LLIR_EXPRESSION_H
