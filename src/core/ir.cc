#include "tcc/core/ir.h"
#include "tcc/core/ir_util.h"
#include "tcc/core/ir_visitor.h"
#include "tcc/common/util.h"
#include <numeric>

namespace tcc {
namespace core {

template<>
void base_expr<var>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<var>(shared_from_this()));
}

template<>
void base_expr<cnst>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<cnst>(shared_from_this()));
}

template<>
void base_expr<range>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<range>(shared_from_this()));
}

template<>
void base_expr<index>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<index>(shared_from_this()));
}

template<>
void base_expr<select>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<select>(shared_from_this()));
}

template<>
void base_expr<reshape>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<reshape>(shared_from_this()));
}

template<>
void base_expr<exp>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<exp>(shared_from_this()));
}

template<>
void base_expr<sqrt>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<sqrt>(shared_from_this()));
}

template<>
void base_expr<add>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<add>(shared_from_this()));
}

template<>
void base_expr<sub>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<sub>(shared_from_this()));
}

template<>
void base_expr<mul>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<mul>(shared_from_this()));
}

template<>
void base_expr<div>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<div>(shared_from_this()));
}

template<>
void base_expr<mod>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<mod>(shared_from_this()));
}

template<>
void base_expr<greater>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<greater>(shared_from_this()));
}

template<>
void base_expr<greater_equal>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<greater_equal>(shared_from_this()));
}

template<>
void base_expr<less>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<less>(shared_from_this()));
}

template<>
void base_expr<logical_and>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<logical_and>(shared_from_this()));
}

template<>
void base_expr<reduce>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<reduce>(shared_from_this()));
}

expr var::make(data_type dtype, dimensions shape)
{
    tcc_assert(!shape.empty(), "shape is empty.");

    std::shared_ptr<var> e(new var);
    e->dtype = dtype;
    e->shape = shape;
    return e;
}

expr cnst::make(std::string data, data_type dtype, dimensions shape)
{
    tcc_assert(!data.empty(), "data is empty.");
    tcc_assert(!shape.empty(), "shape is empty.");

    std::shared_ptr<cnst> e(new cnst);
    e->data = data;
    e->dtype = dtype;
    e->shape = shape;
    return e;
}

expr cnst::make(float f)
{
    std::shared_ptr<cnst> e(new cnst);
    e->data = scalar_serialize<float>(f);
    e->dtype = data_type::FP32;
    return e;
}

expr cnst::make(int64_t i)
{
    std::shared_ptr<cnst> e(new cnst);
    e->data = scalar_serialize<int64_t>(i);
    e->dtype = data_type::INT64;
    return e;
}

expr cnst::make(std::vector<int64_t> is)
{
    std::shared_ptr<cnst> e(new cnst);
    e->data = vector_serialize<int64_t>(is);
    e->dtype = data_type::INT64;
    e->shape = dimensions({ is.size() });
    return e;
}

expr cnst::make(uint64_t ui)
{
    return make(static_cast<int64_t>(ui));
}

expr cnst::make(std::vector<uint64_t> uis)
{
    return make(std::vector<int64_t>(uis.begin(), uis.end()));
}

expr range::make(dimension bound)
{
    tcc_assert(bound > 0, "invalid bound.");

    std::shared_ptr<range> e(new range);
    e->bound = bound;
    e->dtype = data_type::INT64;
    return e;
}

expr index::make(exprs ranges, expr x, exprs indices)
{
    tcc_assert_not_null(x);
    tcc_assert(!ranges.empty(), "ranges is empty.");
    tcc_assert(x->shape.size() == indices.size(),
               "rank of x and size of indices do not agree.");
    tcc_assert(index_validator::apply(ranges, indices),
               "indices contain unspecified range that is not in ranges.");

    std::shared_ptr<index> e(new index);
    e->ranges = ranges;
    e->x = x;
    e->indices = indices;
    e->dtype = x->dtype;
    e->shape = to_shape(ranges);
    return e;
}

expr select::make(exprs ranges, expr cond, expr t, expr f)
{
    tcc_assert_not_null(cond);
    tcc_assert_not_null(t);
    tcc_assert_not_null(f);
    tcc_assert(!ranges.empty(), "ranges is empty.");
    tcc_assert(cond->dtype == data_type::BOOL, "dtype of cond is not BOOL.");
    tcc_assert(t->dtype == f->dtype, "dtypes of t and f do not agree.");
    tcc_assert(t->shape.empty() || (t->type == expr_type::index &&
                                    downcast<index>(t)->ranges == ranges),
               "t is not a scalar or index expr with equivalent ranges.");
    tcc_assert(f->shape.empty() || (f->type == expr_type::index &&
                                    downcast<index>(f)->ranges == ranges),
               "f is not a scalar or index expr with equivalent ranges.");

    std::shared_ptr<select> e(new select);
    e->ranges = ranges;
    e->cond = cond;
    e->t = t;
    e->f = f;
    e->dtype = t->dtype;
    e->shape = to_shape(ranges);
    return e;
}

expr reshape::make(dimensions shape, expr x)
{
    tcc_assert_not_null(x);
    tcc_assert(
        std::accumulate(
            shape.begin(), shape.end(), 1l, std::multiplies<dimension>()) ==
            std::accumulate(x->shape.begin(),
                            x->shape.end(),
                            1l,
                            std::multiplies<dimension>()),
        "shape do not have the same size as shape of x.");

    std::shared_ptr<reshape> e(new reshape);
    e->x = x;
    e->dtype = x->dtype;
    e->shape = shape;
    return e;
}

expr exp::make(expr x)
{
    tcc_assert_not_null(x);

    std::shared_ptr<exp> e(new exp);
    e->x = x;
    e->dtype = x->dtype;
    e->shape = x->shape;
    return e;
}

expr sqrt::make(expr x)
{
    tcc_assert_not_null(x);

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
    tcc_assert(x->dtype == y->dtype, "dtypes of x and y do not agree.");

    std::shared_ptr<logical_and> e(new logical_and);
    e->x = x;
    e->y = y;
    e->dtype = data_type::BOOL;
    e->shape = boardcast(x->shape, y->shape);
    return e;
}

expr reduce::make(type reduce_type,
                  std::unordered_set<unsigned> reduce_dims,
                  expr x)
{
    tcc_assert_not_null(x);
    tcc_assert(!reduce_dims.empty(), "reduce_dims is empty.");

    for (unsigned dim : reduce_dims)
    {
        tcc_assert(dim < x->shape.size(), "reduce dim is out of bound.");
    }

    std::shared_ptr<reduce> e(new reduce);
    e->reduce_type = reduce_type;
    e->reduce_dims = reduce_dims;
    e->x = x;
    e->dtype = x->dtype;

    for (unsigned dim = 0; dim < x->shape.size(); dim++)
    {
        if (reduce_dims.find(dim) == reduce_dims.end())
        {
            e->shape.push_back(x->shape[dim]);
        }
    }

    return e;
}

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

expr operator&&(expr lhs, expr rhs)
{
    return logical_and::make(lhs, rhs);
}

} // namespace core
} // namespace tcc
