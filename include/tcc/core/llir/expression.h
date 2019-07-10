#ifndef TCC_LLIR_EXPRESSION_H
#define TCC_LLIR_EXPRESSION_H

#include <memory>

#include "tcc/core/common/data.h"
#include "tcc/core/common/logging.h"

namespace tcc {
namespace core {

class LLIRVisitor;

/* All LLIR Expr types. */

enum class ExprType {
    Var,
    Const,
    Range,
    Index,
    Add,
    Sub,
    Mul,
    Div,
    GreaterEqual,
    Less,
    And,
    Select,
    Reduce
};

/* Reduction operator types. */

enum class ReduceType {
    SUM
};

/* Base class for LLIR Expression. */

struct BaseExpression {
    BaseExpression(ExprType et) : expr_type(et)  {}

    /* Virtual accept method to support visitor pattern. */
    virtual void accept(LLIRVisitor *v) const = 0;

    ExprType expr_type;
    mutable DataDesc data_desc;
};

typedef std::shared_ptr<const BaseExpression> Expr;

/* Overload arithmetic operators for Expr. */

#define OVERLOAD_OPERATOR(symbol) \
    Expr operator symbol(Expr lhs, Expr rhs); \
    Expr operator symbol(Expr lhs, long rhs);

OVERLOAD_OPERATOR(+)
OVERLOAD_OPERATOR(-)
OVERLOAD_OPERATOR(*)
OVERLOAD_OPERATOR(/)
OVERLOAD_OPERATOR(>=)
OVERLOAD_OPERATOR(<)

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

DECLARE_EXPRESSION(Var)
    static Expr make(DataDesc data_desc_);
END_DECLARE // Var

DECLARE_EXPRESSION(Const)
    Data data;

    static Expr make(Data data_);
END_DECLARE // Const

DECLARE_EXPRESSION(Range)
    std::pair<long, long> range;

    static Expr make(long begin_, long end_);
END_DECLARE // Range

DECLARE_EXPRESSION(Index)
    std::vector<Expr> indices;

    Expr tensor;

    static Expr make(
            std::vector<Expr> indices_,
            Expr tensor_);
END_DECLARE // Index

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

DECLARE_EXPRESSION(Select)
    Expr condition;

    Expr t;
    Expr f;

    static Expr make(
            Expr condition_,
            Expr t_,
            Expr f_);
END_DECLARE // Select

DECLARE_EXPRESSION(Reduce)
    ReduceType reduce_type;
    std::vector<Expr> reduce_axes;

    Expr input;

    static Expr make(
            ReduceType reduce_type_,
            std::vector<Expr> reduce_axes_,
            Expr input_);
END_DECLARE // Reduce

#undef DECLARE_EXPRESSION
#undef END_DECLARE

/* Downcast function for Expr. */
template<typename T> std::shared_ptr<const T> downcast(Expr expr) {
    if (expr && expr->expr_type == T::_expr_type) {
        return std::static_pointer_cast<const T>(expr);
    } else {
        LOG(FATAL) << "Illegal downcast of Expr.";
        return nullptr;
    }
}

/* index function provides a shortcut for expr::Index. */
template<typename ... Args> Expr index(Expr tensor, Args ... args) {
    std::vector<Expr> indices = accumulate_parameters(args...);
    return expr::Index::make(indices, tensor);
}

/* all function applies expr::And to all input boolean LLIR Expressions. */
template<typename T> T all(T expr) { return expr; }
template<typename T, typename ... Args> Expr all(T expr, Args ... args) {
    return And::make(expr, all(args...));
}

} // namespace expr
} // namespace core
} // namespace tcc

#endif // TCC_LLIR_EXPRESSION_H
