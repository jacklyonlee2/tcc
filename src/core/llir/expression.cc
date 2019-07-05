#include "tcc/core/llir/expression.h"

#include "tcc/core/llir/llir.h"

namespace tcc {
namespace core {

/* Expr method implementations. */

#define OVERLOAD_OPERATOR(symbol, expression) \
    Expr Expr::operator symbol (Expr rhs) { \
        return expression::make(*this, rhs); \
    } \
    Expr Expr::operator symbol (long rhs) { \
        return expression::make(*this, expr::Scalar::make(rhs)); \
    }

OVERLOAD_OPERATOR(+, expr::Add)
OVERLOAD_OPERATOR(-, expr::Sub)
OVERLOAD_OPERATOR(*, expr::Mul)
OVERLOAD_OPERATOR(/, expr::Div)
OVERLOAD_OPERATOR(>=, expr::GreaterEqual)
OVERLOAD_OPERATOR(<, expr::Less)

#undef OVERLOAD_OPERATOR

/* Override LLIR Primitive accept method. */

#define IMPLEMENT_ACCEPT(type) \
    template<> void Expression<type>::accept(LLIRVisitor *v) const { \
        v->visit(expr::downcast<type>(shared_from_this())); \
    }

IMPLEMENT_ACCEPT(expr::Scalar)
IMPLEMENT_ACCEPT(expr::Range)
IMPLEMENT_ACCEPT(expr::Add)
IMPLEMENT_ACCEPT(expr::Sub)
IMPLEMENT_ACCEPT(expr::Mul)
IMPLEMENT_ACCEPT(expr::Div)
IMPLEMENT_ACCEPT(expr::GreaterEqual)
IMPLEMENT_ACCEPT(expr::Less)
IMPLEMENT_ACCEPT(expr::And)

#undef IMPLEMENT_ACCEPT

namespace expr {

Expr Scalar::make(long value_) {
    std::shared_ptr<Scalar> expr(new Scalar);
    expr->value_type = ValueType::LONG;
    expr->long_value = value_;
    return expr;
}

Expr Range::make(long begin_, long end_) {
    CHECK(end_ >= begin_) << "end_ must be bigger or equal to begin_.";

    std::shared_ptr<Range> expr(new Range);
    expr->value_type = ValueType::INDICES;
    expr->range = std::pair<long,long>({begin_, end_});
    return expr;
}

Expr Add::make(Expr x_, Expr y_) {
    CHECK(x_->value_type == y_->value_type) <<
        "Binary value types must agree.";

    std::shared_ptr<Add> expr(new Add);
    expr->value_type = x_->value_type;
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr Sub::make(Expr x_, Expr y_) {
    CHECK(x_->value_type == y_->value_type) <<
        "Binary value types must agree.";

    std::shared_ptr<Sub> expr(new Sub);
    expr->value_type = x_->value_type;
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr Mul::make(Expr x_, Expr y_) {
    CHECK(x_->value_type == y_->value_type) <<
        "Binary value types must agree.";

    std::shared_ptr<Mul> expr(new Mul);
    expr->value_type = x_->value_type;
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr Div::make(Expr x_, Expr y_) {
    CHECK(x_->value_type == y_->value_type) <<
        "Binary value types must agree.";

    std::shared_ptr<Div> expr(new Div);
    expr->value_type = x_->value_type;
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr GreaterEqual::make(Expr x_, Expr y_) {
    CHECK(x_->value_type == y_->value_type) <<
        "Binary value types must agree.";

    std::shared_ptr<GreaterEqual> expr(new GreaterEqual);
    expr->value_type = ValueType::BOOL;
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr Less::make(Expr x_, Expr y_) {
    CHECK(x_->value_type == y_->value_type) <<
        "Binary value types must agree.";

    std::shared_ptr<Less> expr(new Less);
    expr->value_type = ValueType::BOOL;
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr And::make(Expr x_, Expr y_) {
    CHECK(x_->value_type == ValueType::BOOL) <<
        "x_ must be of type ValueType::BOOL.";
    CHECK(y_->value_type == ValueType::BOOL) <<
        "y_ must be of type ValueType::BOOL.";

    std::shared_ptr<And> expr(new And);
    expr->value_type = ValueType::BOOL;
    expr->x = x_;
    expr->y = y_;
    return expr;
}

} // namespace expr
} // namespace core
} // namespace tcc
