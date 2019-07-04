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
    INDICES
};

/* All LLIR Expr types. */

enum class ExprType {
    Range
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

DECLARE_EXPRESSION(Range)
    std::pair<long, long> range;

    static Expr make(long bound_);
END_DECLARE // Range

/* Helper functions for Expr. */

template<typename T> std::shared_ptr<const T> downcast(Expr expr) {
    if (expr && expr->expr_type == T::_expr_type) {
        return std::static_pointer_cast<const T>(expr);
    } else {
        LOG(FATAL) << "Illegal downcast of Expr.";
        return nullptr;
    }
}

struct Ranges : public std::vector<RangePtr> {
    using std::vector<RangePtr>::vector;

    static Ranges from_shape(std::vector<long> shape) {
        Ranges ranges;
        for (long dim : shape) {
            ranges.push_back(downcast<Range>(Range::make(dim)));
        }
        return ranges;
    }

    std::vector<long> to_shape() const {
        std::vector<long> shape;
        for (RangePtr range : *this) {
            long dim = range->range.second - range->range.first;
            CHECK(dim > 0) << "Dimension must be positive.";
            shape.push_back(dim);
        }
        return shape;
    }
};

} // namespace expr
} // namespace core
} // namespace tcc

#endif // TCC_LLIR_EXPRESSION_H
