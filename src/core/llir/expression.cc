#include "tcc/core/llir/expression.h"

#include "tcc/core/llir/llir.h"

namespace tcc {
namespace core {

/* Expr method implementations.
 * overloading arithmetic operators. */

template<typename ... Args>
Expr BaseExpression::operator()(Args ... args) const {
}

#define OVERLOAD_OPERATOR(symbol, expression) \
    Expr operator symbol(Expr lhs, Expr rhs) { \
        return expression::make(lhs, rhs); \
    } \
    Expr operator symbol(Expr lhs, long rhs) { \
        return expression::make(lhs, expr::Const::make(rhs)); \
    }

OVERLOAD_OPERATOR(+, expr::Add)
OVERLOAD_OPERATOR(-, expr::Sub)
OVERLOAD_OPERATOR(*, expr::Mul)
OVERLOAD_OPERATOR(/, expr::Div)
OVERLOAD_OPERATOR(>=, expr::GreaterEqual)
OVERLOAD_OPERATOR(<, expr::Less)

#undef OVERLOAD_OPERATOR

/* Override LLIR Expression accept method. */

#define IMPLEMENT_ACCEPT(type) \
    template<> void Expression<type>::accept(LLIRVisitor *v) const { \
        v->visit(expr::downcast<type>(shared_from_this())); \
    }

IMPLEMENT_ACCEPT(expr::Var)
IMPLEMENT_ACCEPT(expr::Const)
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


Expr Var::make(DataDesc data_desc_) {
    CHECK(data_desc_.defined());

    std::shared_ptr<Var> expr(new Var);
    expr->data_desc = data_desc_;
    return expr;
}

Expr Const::make(Data data_) {
    CHECK(data_.defined());

    std::shared_ptr<Const> expr(new Const);
    expr->data_desc = data_;
    expr->data = data_;
    return expr;
}

Expr Range::make(long begin_, long end_) {
    CHECK(end_ >= begin_);

    std::shared_ptr<Range> expr(new Range);
    expr->data_desc = DataType::LONG;
    expr->range = std::pair<long,long>({begin_, end_});
    return expr;
}

Expr Add::make(Expr x_, Expr y_) {
    CHECK_NOTNULL(x_);
    CHECK_NOTNULL(y_);
    CHECK(x_->data_desc.get_type() == y_->data_desc.get_type());

    std::shared_ptr<Add> expr(new Add);
    expr->data_desc = x_->data_desc;
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr Sub::make(Expr x_, Expr y_) {
    CHECK_NOTNULL(x_);
    CHECK_NOTNULL(y_);
    CHECK(x_->data_desc.get_type() == y_->data_desc.get_type());

    std::shared_ptr<Sub> expr(new Sub);
    expr->data_desc = x_->data_desc;
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr Mul::make(Expr x_, Expr y_) {
    CHECK_NOTNULL(x_);
    CHECK_NOTNULL(y_);
    CHECK(x_->data_desc.get_type() == y_->data_desc.get_type());

    std::shared_ptr<Mul> expr(new Mul);
    expr->data_desc = x_->data_desc;
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr Div::make(Expr x_, Expr y_) {
    CHECK_NOTNULL(x_);
    CHECK_NOTNULL(y_);
    CHECK(x_->data_desc.get_type() == y_->data_desc.get_type());

    std::shared_ptr<Div> expr(new Div);
    expr->data_desc = x_->data_desc;
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr GreaterEqual::make(Expr x_, Expr y_) {
    CHECK_NOTNULL(x_);
    CHECK_NOTNULL(y_);
    CHECK(x_->data_desc.get_type() == y_->data_desc.get_type());

    std::shared_ptr<GreaterEqual> expr(new GreaterEqual);
    expr->data_desc = DataType::BOOL;
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr Less::make(Expr x_, Expr y_) {
    CHECK_NOTNULL(x_);
    CHECK_NOTNULL(y_);
    CHECK(x_->data_desc.get_type() == y_->data_desc.get_type());

    std::shared_ptr<Less> expr(new Less);
    expr->data_desc = DataType::BOOL;
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr And::make(Expr x_, Expr y_) {
    CHECK_NOTNULL(x_);
    CHECK_NOTNULL(y_);
    CHECK(x_->data_desc.get_type() == DataType::BOOL);
    CHECK(y_->data_desc.get_type() == DataType::BOOL);
    CHECK(x_->data_desc.scalar());
    CHECK(y_->data_desc.scalar());

    std::shared_ptr<And> expr(new And);
    expr->data_desc = DataType::BOOL;
    expr->x = x_;
    expr->y = y_;
    return expr;
}

} // namespace expr
} // namespace core
} // namespace tcc
