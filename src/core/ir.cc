#include "tcc/core/ir.h"
#include "tcc/core/ir_util.h"
#include "tcc/core/ir_visitor.h"

namespace tcc {

expr var::make(datatype dtype, dimensions shape)
{
    tcc_assert(!shape.empty(), "shape is empty.");

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
    tcc_assert(cond->dtype == datatype::BOOL, "dtype of cond is not BOOL.");
    tcc_assert(t->dtype == f->dtype, "dtypes of t and f do not agree.");
    tcc_assert(t->shape.empty() || (t->type == exprtype::index &&
                                    downcast<index>(t)->ranges == ranges),
               "t is not a scalar or index expr with equivalent ranges.");
    tcc_assert(f->shape.empty() || (f->type == exprtype::index &&
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

    std::shared_ptr<binary> e(new binary);
    e->binary_type = binary_type;
    e->x = x;
    e->y = y;
    e->dtype = x->dtype;
    e->shape = boardcast(x->shape, y->shape);
    return e;
}

expr logical::make(type logical_type, expr x, expr y)
{
    tcc_assert_not_null(x);
    tcc_assert_not_null(y);
    tcc_assert(x->dtype == y->dtype, "dtypes of x and y do not agree.");

    std::shared_ptr<logical> e(new logical);
    e->logical_type = logical_type;
    e->x = x;
    e->y = y;
    e->dtype = datatype::BOOL;
    e->shape = boardcast(x->shape, y->shape);
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

template<>
void base_expr<logical>::accept(std::shared_ptr<ir_visitor> v) const
{
    v->visit(downcast<logical>(shared_from_this()));
}

} // namespace tcc
