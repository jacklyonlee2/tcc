#include "tcc/core/ir.h"
#include "tcc/core/ir_util.h"
#include "tcc/core/ir_visitor.h"
#include <algorithm>

namespace tcc {

expr var::make(datatype dtype, dimensions shape)
{
    std::shared_ptr<var> e(new var);
    e->dtype = dtype;
    e->shape = shape;
    return e;
}

expr cnst::make(std::string data, datatype dtype, dimensions shape)
{
    tcc_assert(!data.empty(), "data is empty.");
    tcc_assert(!shape.empty(), "shape is empty.");

    std::shared_ptr<cnst> e(new cnst);
    e->data = data;
    e->dtype = dtype;
    e->shape = shape;
    return e;
}

expr range::make(dimension bound)
{
    tcc_assert(bound > 0, "invalid bound.");

    std::shared_ptr<range> e(new range);
    e->bound = bound;
    e->dtype = datatype::INT64;
    return e;
}

expr index::make(exprs ranges, expr x, exprs indices)
{
    tcc_assert_not_null(x);
    tcc_assert(!ranges.empty(), "ranges is empty.");
    tcc_assert(!x->shape.empty(), "x is a scalar.");
    tcc_assert(x->shape.size() == indices.size(),
               "rank of x and size of indices do not agree.");
    tcc_assert(index_validator::apply(ranges, indices),
               "indices contain unspecified range that is not in ranges.");
    tcc_assert(!(ranges.size() == indices.size() && ranges == indices),
               "redundant index expr (element-wise index).");

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
    tcc_assert(cond->dtype == datatype::BOOL, "dtype of cond is not BOOL.");
    tcc_assert(t->dtype == f->dtype, "dtypes of t and f do not agree.");
    tcc_assert(cond->shape.empty() || cond->shape == to_shape(ranges),
               "cond is not a scalar or expr with equivalent shape.");
    tcc_assert(t->shape.empty() || t->shape == to_shape(ranges) ||
                   (t->type == exprtype::index &&
                    downcast<index>(t)->ranges == ranges),
               "t is not a scalar, expr with equivalent shape, or index expr "
               "with equivalent ranges.");
    tcc_assert(f->shape.empty() || f->shape == to_shape(ranges) ||
                   (f->type == exprtype::index &&
                    downcast<index>(f)->ranges == ranges),
               "f is not a scalar, expr with equivalent shape, or index expr "
               "with equivalent ranges.");

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

expr reduce::make(type reduce_type,
                  std::unordered_set<unsigned> reduce_dims,
                  expr x)
{
    tcc_assert_not_null(x);
    tcc_assert(!x->shape.empty(), "x is scalar.");
    tcc_assert(!reduce_dims.empty(), "reduce_dims is empty.");

    dimension reduce_size = std::accumulate(
        reduce_dims.begin(),
        reduce_dims.end(),
        1,
        [&](dimension sz, unsigned i) {
            tcc_assert(i < x->shape.size(), "i is out of bound.");
            return sz *= x->shape[i];
        });

    dimensions shape;
    for (unsigned i = 0; i < x->shape.size(); i++)
    {
        if (reduce_dims.find(i) == reduce_dims.end())
        {
            shape.push_back(x->shape[i]);
        }
    }

    std::shared_ptr<reduce> e(new reduce);
    e->reduce_type = reduce_type;
    e->reduce_dims = reduce_dims;
    e->reduce_size = reduce_size;
    e->x = x;
    e->dtype = x->dtype;
    e->shape = shape;
    return e;
}

expr unary::make(type unary_type, expr x)
{
    tcc_assert_not_null(x);

    std::shared_ptr<unary> e(new unary);
    e->unary_type = unary_type;
    e->x = x;
    e->dtype = x->dtype;
    e->shape = x->shape;
    return e;
}

expr binary::make(type binary_type, expr x, expr y)
{
    tcc_assert_not_null(x);
    tcc_assert_not_null(y);
    tcc_assert(x->dtype == y->dtype, "dtypes of x and y do not agree.");
    tcc_assert(x->shape.size() >= y->shape.size(),
               "y can not be boardcasted to x.");

    /* broadcast non-scalar inputs with non-equivalent shapes. */
    if (!x->shape.empty() && !y->shape.empty() && x->shape != y->shape)
    {
        exprs x_ranges = to_ranges(x->shape), y_ranges;
        unsigned idiff = x->shape.size() - y->shape.size();
        for (unsigned i = 0; i < y->shape.size(); i++)
        {
            if (y->shape[i] == 1)
            {
                y_ranges.push_back(cnst::make(0l));
            }
            else if (x->shape[i + idiff] == y->shape[i])
            {
                y_ranges.push_back(x_ranges[i + idiff]);
            }
            else
            {
                tcc_error("y can not be boardcasted to x.");
            }
        }
        y = index::make(x_ranges, y, y_ranges);
    }

    datatype dtype = ([&]() {
        switch (binary_type)
        {
            case binary::type::add:
            case binary::type::sub:
            case binary::type::mul:
            case binary::type::div:
            case binary::type::mod:
                return x->dtype;
            case binary::type::logical_and:
            case binary::type::greater:
            case binary::type::greater_eq:
            case binary::type::less:
                return datatype::BOOL;
            default:
                tcc_error("unknown binary type.");
        }
    }());

    std::shared_ptr<binary> e(new binary);
    e->binary_type = binary_type;
    e->x = x;
    e->y = y;
    e->dtype = dtype;
    e->shape = x->shape;
    return e;
}

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
void base_expr<reduce>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<reduce>(shared_from_this()));
}

template<>
void base_expr<unary>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<unary>(shared_from_this()));
}

template<>
void base_expr<binary>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<binary>(shared_from_this()));
}

} // namespace tcc
