#include "tcc/hlir/ir.h"
#include "tcc/common/util.h"
#include "tcc/hlir/visitor.h"

namespace tcc {
namespace hlir {

/* hlir primitive implementations. */

expr var::make(data_type dtype, std::vector<long> shape)
{
    tcc_assert(dtype != data_type::VOID, "dtype is uninitialized.");
    tcc_assert(!shape.empty(), "shape is empty.");

    std::shared_ptr<var> e(new var);
    e->dtype = dtype;
    e->shape = shape;
    return e;
}

expr cnst::make(const std::string data,
                data_type dtype,
                std::vector<long> shape)
{
    tcc_assert(!data.empty(), "data is empty.");
    tcc_assert(dtype != data_type::VOID, "dtype is uninitialized.");
    tcc_assert(!shape.empty(), "shape is empty.");

    std::shared_ptr<cnst> e(new cnst);
    e->data = data;
    e->dtype = dtype;
    e->shape = shape;
    return e;
}

expr cnst::make(const float f)
{
    std::shared_ptr<cnst> e(new cnst);
    e->data = scalar_serialize<float>(f);
    e->dtype = data_type::FP32;
    return e;
}

expr cnst::make(const int i)
{
    std::shared_ptr<cnst> e(new cnst);
    e->data = scalar_serialize<int>(i);
    e->dtype = data_type::INT32;
    return e;
}

expr exp::make(expr x)
{
    tcc_assert_not_null(x);
    tcc_assert(x->dtype != data_type::VOID, "dtype of x is uninitialized.");

    std::shared_ptr<exp> e(new exp);
    e->x = x;
    e->dtype = x->dtype;
    e->shape = x->shape;
    return e;
}

expr sqrt::make(expr x)
{
    tcc_assert_not_null(x);
    tcc_assert(x->dtype != data_type::VOID, "dtype of x is uninitialized.");

    std::shared_ptr<sqrt> e(new sqrt);
    e->x = x;
    e->dtype = x->dtype;
    e->shape = x->shape;
    return e;
}

expr add::make(expr x, expr y)
{
    tcc_assert_not_null(x);
    tcc_assert_not_null(y);
    tcc_assert(x->dtype != data_type::VOID, "dtype of x is uninitialized.");
    tcc_assert(y->dtype != data_type::VOID, "dtype of y is uninitialized.");
    tcc_assert(x->dtype == y->dtype, "dtypes of x and y do not agree.");

    std::shared_ptr<add> e(new add);
    e->x = x;
    e->y = y;
    e->dtype = x->dtype;
    e->shape = boardcast(x->shape, y->shape);
    return e;
}

expr sub::make(expr x, expr y)
{
    tcc_assert_not_null(x);
    tcc_assert_not_null(y);
    tcc_assert(x->dtype != data_type::VOID, "dtype of x is uninitialized.");
    tcc_assert(y->dtype != data_type::VOID, "dtype of y is uninitialized.");
    tcc_assert(x->dtype == y->dtype, "dtypes of x and y do not agree.");

    std::shared_ptr<sub> e(new sub);
    e->x = x;
    e->y = y;
    e->dtype = x->dtype;
    e->shape = boardcast(x->shape, y->shape);
    return e;
}

expr mul::make(expr x, expr y)
{
    tcc_assert_not_null(x);
    tcc_assert_not_null(y);
    tcc_assert(x->dtype != data_type::VOID, "dtype of x is uninitialized.");
    tcc_assert(y->dtype != data_type::VOID, "dtype of y is uninitialized.");
    tcc_assert(x->dtype == y->dtype, "dtypes of x and y do not agree.");

    std::shared_ptr<mul> e(new mul);
    e->x = x;
    e->y = y;
    e->dtype = x->dtype;
    e->shape = boardcast(x->shape, y->shape);
    return e;
}

expr div::make(expr x, expr y)
{
    tcc_assert_not_null(x);
    tcc_assert_not_null(y);
    tcc_assert(x->dtype != data_type::VOID, "dtype of x is uninitialized.");
    tcc_assert(y->dtype != data_type::VOID, "dtype of y is uninitialized.");
    tcc_assert(x->dtype == y->dtype, "dtypes of x and y do not agree.");

    std::shared_ptr<div> e(new div);
    e->x = x;
    e->y = y;
    e->dtype = x->dtype;
    e->shape = boardcast(x->shape, y->shape);
    return e;
}

expr mod::make(expr x, expr y)
{
    tcc_assert_not_null(x);
    tcc_assert_not_null(y);
    tcc_assert(x->dtype != data_type::VOID, "dtype of x is uninitialized.");
    tcc_assert(y->dtype != data_type::VOID, "dtype of y is uninitialized.");
    tcc_assert(x->dtype == y->dtype, "dtypes of x and y do not agree.");

    std::shared_ptr<mod> e(new mod);
    e->x = x;
    e->y = y;
    e->dtype = x->dtype;
    e->shape = boardcast(x->shape, y->shape);
    return e;
}

expr greater::make(expr x, expr y)
{
    tcc_assert_not_null(x);
    tcc_assert_not_null(y);
    tcc_assert(x->dtype != data_type::VOID, "dtype of x is uninitialized.");
    tcc_assert(y->dtype != data_type::VOID, "dtype of y is uninitialized.");
    tcc_assert(x->dtype == y->dtype, "dtypes of x and y do not agree.");

    std::shared_ptr<greater> e(new greater);
    e->x = x;
    e->y = y;
    e->dtype = data_type::BOOL;
    e->shape = boardcast(x->shape, y->shape);
    return e;
}

expr greater_equal::make(expr x, expr y)
{
    tcc_assert_not_null(x);
    tcc_assert_not_null(y);
    tcc_assert(x->dtype != data_type::VOID, "dtype of x is uninitialized.");
    tcc_assert(y->dtype != data_type::VOID, "dtype of y is uninitialized.");
    tcc_assert(x->dtype == y->dtype, "dtypes of x and y do not agree.");

    std::shared_ptr<greater_equal> e(new greater_equal);
    e->x = x;
    e->y = y;
    e->dtype = data_type::BOOL;
    e->shape = boardcast(x->shape, y->shape);
    return e;
}

expr less::make(expr x, expr y)
{
    tcc_assert_not_null(x);
    tcc_assert_not_null(y);
    tcc_assert(x->dtype != data_type::VOID, "dtype of x is uninitialized.");
    tcc_assert(y->dtype != data_type::VOID, "dtype of y is uninitialized.");
    tcc_assert(x->dtype == y->dtype, "dtypes of x and y do not agree.");

    std::shared_ptr<less> e(new less);
    e->x = x;
    e->y = y;
    e->dtype = data_type::BOOL;
    e->shape = boardcast(x->shape, y->shape);
    return e;
}

expr logical_and::make(expr x, expr y)
{
    tcc_assert_not_null(x);
    tcc_assert_not_null(y);
    tcc_assert(x->dtype != data_type::VOID, "dtype of x is uninitialized.");
    tcc_assert(y->dtype != data_type::VOID, "dtype of y is uninitialized.");
    tcc_assert(x->dtype == y->dtype, "dtypes of x and y do not agree.");

    std::shared_ptr<logical_and> e(new logical_and);
    e->x = x;
    e->y = y;
    e->dtype = data_type::BOOL;
    e->shape = boardcast(x->shape, y->shape);
    return e;
}

/* overload arithmetic and logical operators for expr. */

expr operator+(expr lhs, expr rhs)
{
    return add::make(lhs, rhs);
}

expr operator-(expr lhs, expr rhs)
{
    return sub::make(lhs, rhs);
}

expr operator*(expr lhs, expr rhs)
{
    return mul::make(lhs, rhs);
}

expr operator/(expr lhs, expr rhs)
{
    return div::make(lhs, rhs);
}

expr operator%(expr lhs, expr rhs)
{
    return mod::make(lhs, rhs);
}

expr operator>(expr lhs, expr rhs)
{
    return greater::make(lhs, rhs);
}

expr operator>=(expr lhs, expr rhs)
{
    return greater_equal::make(lhs, rhs);
}

expr operator<(expr lhs, expr rhs)
{
    return less::make(lhs, rhs);
}

/* overridden expr_template accept methods. */

template<>
void expr_template<var>::accept(visitor* v) const
{
    v->visit(downcast<var>(shared_from_this()));
}

template<>
void expr_template<cnst>::accept(visitor* v) const
{
    v->visit(downcast<cnst>(shared_from_this()));
}

template<>
void expr_template<range>::accept(visitor* v) const
{
    v->visit(downcast<range>(shared_from_this()));
}

template<>
void expr_template<index>::accept(visitor* v) const
{
    v->visit(downcast<index>(shared_from_this()));
}

template<>
void expr_template<exp>::accept(visitor* v) const
{
    v->visit(downcast<exp>(shared_from_this()));
}

template<>
void expr_template<sqrt>::accept(visitor* v) const
{
    v->visit(downcast<sqrt>(shared_from_this()));
}

template<>
void expr_template<add>::accept(visitor* v) const
{
    v->visit(downcast<add>(shared_from_this()));
}

template<>
void expr_template<sub>::accept(visitor* v) const
{
    v->visit(downcast<sub>(shared_from_this()));
}

template<>
void expr_template<mul>::accept(visitor* v) const
{
    v->visit(downcast<mul>(shared_from_this()));
}

template<>
void expr_template<div>::accept(visitor* v) const
{
    v->visit(downcast<div>(shared_from_this()));
}

template<>
void expr_template<mod>::accept(visitor* v) const
{
    v->visit(downcast<mod>(shared_from_this()));
}

template<>
void expr_template<greater>::accept(visitor* v) const
{
    v->visit(downcast<greater>(shared_from_this()));
}

template<>
void expr_template<greater_equal>::accept(visitor* v) const
{
    v->visit(downcast<greater_equal>(shared_from_this()));
}

template<>
void expr_template<less>::accept(visitor* v) const
{
    v->visit(downcast<less>(shared_from_this()));
}

template<>
void expr_template<logical_and>::accept(visitor* v) const
{
    v->visit(downcast<logical_and>(shared_from_this()));
}

template<>
void expr_template<select>::accept(visitor* v) const
{
    v->visit(downcast<select>(shared_from_this()));
}

template<>
void expr_template<reduce>::accept(visitor* v) const
{
    v->visit(downcast<reduce>(shared_from_this()));
}

} // namespace hlir
} // namespace tcc
