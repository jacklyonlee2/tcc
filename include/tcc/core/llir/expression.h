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
};

/* All LLIR Expr types. */

enum class ExprType {
};

/* Base class for LLIR Expression. */

struct BaseExpression {
    BaseExpression(ExprType et, ValueType vt) :
        expr_type(et), value_type(vt) {}

    /* Virtual accept method to support visitor pattern. */
    virtual void accept(LLIRVisitor *v) const = 0;

    ExprType expr_type;
    ValueType value_type;
};

typedef std::shared_ptr<const BaseExpression> Expr;

/* Templated LLIR Expression class.
 * LLIR Expressions are used to express the access pattern
 * and conditions of LLIR Primitive's inputs.
 * LLIR Expressions are store inside LLIR Primitive
 * along side of the Primitive inputs. */

template<typename T>
struct Expression :
    public BaseExpression,
    public std::enable_shared_from_this<Expression<T>> {
    Expression(ValueType vt) : BaseExpression(T::_expr_type, vt) {}

    void accept(LLIRVisitor *v) const override;
};

namespace expr {

/* Helper function to downcast Expr. */

template<typename T> std::shared_ptr<const T> downcast(Expr expr) {
    if (expr && expr->expr_type == T::_expr_type) {
        return std::static_pointer_cast<const T>(expr);
    } else {
        LOG(FATAL) << "Illegal downcast of Expr.";
        return nullptr;
    }
}

/* --- Declare LLIR Exprs --- */

#define DECLARE_EXPRESSION(type) \
    struct type; \
    typedef std::shared_ptr<const type> type##Ptr; \
    struct type : public Expression<type> { \
        using Expression::Expression; \
        static const ExprType _expr_type = ExprType::type;
#define END_DECLARE };

#undef DECLARE_EXPRESSION
#undef END_DECLARE

} // namespace expr
} // namespace core
} // namespace tcc

#endif // TCC_LLIR_EXPRESSION_H
