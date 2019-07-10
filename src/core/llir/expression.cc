#include "tcc/core/llir/expression.h"

#include "tcc/core/llir/llir.h"
#include "tcc/core/common/util.h"

namespace tcc {
namespace core {

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
IMPLEMENT_ACCEPT(expr::Index)
IMPLEMENT_ACCEPT(expr::Add)
IMPLEMENT_ACCEPT(expr::Sub)
IMPLEMENT_ACCEPT(expr::Mul)
IMPLEMENT_ACCEPT(expr::Div)
IMPLEMENT_ACCEPT(expr::GreaterEqual)
IMPLEMENT_ACCEPT(expr::Less)
IMPLEMENT_ACCEPT(expr::And)
IMPLEMENT_ACCEPT(expr::Select)
IMPLEMENT_ACCEPT(expr::Reduce)

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
    expr->range = { begin_, end_ };
    return expr;
}

Expr Index::make(
        std::vector<Expr> indices_,
        Expr tensor_) {
    for (Expr index : indices_) {
        CHECK_NOTNULL(index);
        CHECK(index->data_desc.defined());
        CHECK(index->data_desc.get_type() == DataType::LONG) <<
            "Index datatype must be DataType::LONG.";
    }

    CHECK_NOTNULL(tensor_);
    CHECK(tensor_->data_desc.defined());
    CHECK(indices_.size() == tensor_->data_desc.get_rank()) <<
        "Size of indices_ must equal to tensor_ rank.";

    std::shared_ptr<Index> expr(new Index);
    expr->data_desc = tensor_->data_desc.get_type();
    expr->indices = indices_;
    expr->tensor = tensor_;
    return expr;
}

Expr Add::make(Expr x_, Expr y_) {
    CHECK_NOTNULL(x_);
    CHECK_NOTNULL(y_);
    CHECK(x_->data_desc.defined() && y_->data_desc.defined());
    CHECK(x_->data_desc.get_type() == y_->data_desc.get_type());

    std::shared_ptr<Add> expr(new Add);
    expr->data_desc = x_->data_desc.get_type();
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr Sub::make(Expr x_, Expr y_) {
    CHECK_NOTNULL(x_);
    CHECK_NOTNULL(y_);
    CHECK(x_->data_desc.defined() && y_->data_desc.defined());
    CHECK(x_->data_desc.get_type() == y_->data_desc.get_type());

    std::shared_ptr<Sub> expr(new Sub);
    expr->data_desc = x_->data_desc.get_type();
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr Mul::make(Expr x_, Expr y_) {
    CHECK_NOTNULL(x_);
    CHECK_NOTNULL(y_);
    CHECK(x_->data_desc.defined() && y_->data_desc.defined());

    std::shared_ptr<Mul> expr(new Mul);
    expr->data_desc = x_->data_desc.get_type();
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr Div::make(Expr x_, Expr y_) {
    CHECK_NOTNULL(x_);
    CHECK_NOTNULL(y_);
    CHECK(x_->data_desc.defined() && y_->data_desc.defined());
    CHECK(x_->data_desc.get_type() == y_->data_desc.get_type());

    std::shared_ptr<Div> expr(new Div);
    expr->data_desc = x_->data_desc.get_type();
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr GreaterEqual::make(Expr x_, Expr y_) {
    CHECK_NOTNULL(x_);
    CHECK_NOTNULL(y_);
    CHECK(x_->data_desc.defined() && y_->data_desc.defined());
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
    CHECK(x_->data_desc.defined() && y_->data_desc.defined());
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
    CHECK(x_->data_desc.defined() && y_->data_desc.defined());
    CHECK(x_->data_desc.get_type() == DataType::BOOL) <<
        "x_ and y_ must be of type DataType::BOOL.";

    std::shared_ptr<And> expr(new And);
    expr->data_desc = DataType::BOOL;
    expr->x = x_;
    expr->y = y_;
    return expr;
}

Expr Select::make(
        Expr condition_,
        Expr t_,
        Expr f_) {
    CHECK_NOTNULL(condition_);
    CHECK_NOTNULL(t_);
    CHECK_NOTNULL(f_);
    CHECK(condition_->data_desc.defined() &&
            t_->data_desc.defined() &&
            f_->data_desc.defined());
    CHECK(condition_->data_desc.get_type() == DataType::BOOL) <<
        "condition_ must be of type DataType::BOOL.";

    std::shared_ptr<Select> expr(new Select);
    expr->data_desc = t_->data_desc.get_type();
    expr->condition = condition_;
    expr->t = t_;
    expr->f = f_;
    return expr;
}

Expr Reduce::make(
        ReduceType reduce_type_,
        std::vector<Expr> reduce_axes_,
        Expr input_) {
    for (Expr axis : reduce_axes_) {
        CHECK_NOTNULL(axis);
        CHECK(axis->data_desc.defined());
        CHECK(axis->expr_type == ExprType::Range) <<
            "axis must be expr::Range.";
    }

    CHECK_NOTNULL(input_);
    CHECK(input_->data_desc.defined());

    std::shared_ptr<Reduce> expr(new Reduce);
    expr->data_desc = input_->data_desc.get_type();
    expr->reduce_type = reduce_type_;
    expr->reduce_axes = reduce_axes_;
    expr->input = input_;
    return expr;
}

} // namespace expr
} // namespace core
} // namespace tcc
