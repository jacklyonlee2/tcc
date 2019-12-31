#include "tcc/hlir/ir.h"
#include "tcc/common/util.h"
#include "tcc/hlir/index_validator.h"
#include "tcc/hlir/visitor.h"

namespace tcc {
namespace hlir {

/* hlir primitive implementations. */

expr var::make(data_type dtype, std::vector<long> shape)
{
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

expr cnst::make(const long l)
{
    std::shared_ptr<cnst> e(new cnst);
    e->data = scalar_serialize<long>(l);
    e->dtype = data_type::INT32;
    return e;
}

expr range::make(const long bound)
{
    tcc_assert(bound > 0, "invalid bound.");

    std::shared_ptr<range> e(new range);
    e->bound = bound;
    e->dtype = data_type::INT32;
    return e;
}

expr index::make(ranges rs, expr x, exprs indices)
{
    tcc_assert_not_null(x);
    tcc_assert(!rs.empty(), "rs is empty.");
    tcc_assert(x->shape.size() == indices.size(),
               "rank of x and size of indices do not agree.");

    /* validate indices does not contain any unspecified range exprs. */
    index_validator::make(rs)->visit(indices);

    std::shared_ptr<index> e(new index);
    e->rs = rs;
    e->x = x;
    e->indices = indices;
    e->dtype = x->dtype;
    e->shape = to_shape(rs);
    return e;
}

expr select::make(ranges rs, expr cond, expr t, expr f)
{
    tcc_assert_not_null(cond);
    tcc_assert_not_null(t);
    tcc_assert_not_null(f);
    tcc_assert(!rs.empty(), "rs is empty.");
    tcc_assert(cond->dtype == data_type::BOOL, "dtype of cond is not BOOL.");
    tcc_assert(t->dtype == f->dtype, "dtypes of t and f do not agree.");
    tcc_assert(t->shape.empty() || (t->type == expr_type::index &&
                                    downcast<index>(t)->rs == rs),
               "t is not a scalar or index expr with equivalent ranges.");
    tcc_assert(f->shape.empty() || (f->type == expr_type::index &&
                                    downcast<index>(f)->rs == rs),
               "f is not a scalar or index expr with equivalent ranges.");

    std::shared_ptr<select> e(new select);
    e->rs = rs;
    e->t = t;
    e->f = f;
    e->dtype = t->dtype;
    e->shape = to_shape(rs);
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

expr reduce::make(reduce_type rtype, std::unordered_set<unsigned> rdims, expr x)
{
    tcc_assert_not_null(x);
    tcc_assert(!rdims.empty(), "rdim is empty.");

    for (unsigned rdim : rdims)
    {
        tcc_assert(rdim < x->shape.size(), "rdim is out of bound.");
    }

    std::shared_ptr<reduce> e(new reduce);
    e->rtype = rtype;
    e->rdims = rdims;
    e->x = x;
    e->dtype = x->dtype;

    for (unsigned dim = 0; dim < x->shape.size(); dim++)
    {
        if (rdims.find(dim) == rdims.end())
        {
            e->shape.push_back(x->shape[dim]);
        }
    }

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

expr operator&&(expr lhs, expr rhs)
{
    return logical_and::make(lhs, rhs);
}

/* overridden expr_template accept methods. */

template<>
void expr_template<var>::accept(visitor v) const
{
    v->visit(downcast<var>(shared_from_this()));
}

template<>
void expr_template<cnst>::accept(visitor v) const
{
    v->visit(downcast<cnst>(shared_from_this()));
}

template<>
void expr_template<range>::accept(visitor v) const
{
    v->visit(downcast<range>(shared_from_this()));
}

template<>
void expr_template<index>::accept(visitor v) const
{
    v->visit(downcast<index>(shared_from_this()));
}

template<>
void expr_template<select>::accept(visitor v) const
{
    v->visit(downcast<select>(shared_from_this()));
}

template<>
void expr_template<exp>::accept(visitor v) const
{
    v->visit(downcast<exp>(shared_from_this()));
}

template<>
void expr_template<sqrt>::accept(visitor v) const
{
    v->visit(downcast<sqrt>(shared_from_this()));
}

template<>
void expr_template<add>::accept(visitor v) const
{
    v->visit(downcast<add>(shared_from_this()));
}

template<>
void expr_template<sub>::accept(visitor v) const
{
    v->visit(downcast<sub>(shared_from_this()));
}

template<>
void expr_template<mul>::accept(visitor v) const
{
    v->visit(downcast<mul>(shared_from_this()));
}

template<>
void expr_template<div>::accept(visitor v) const
{
    v->visit(downcast<div>(shared_from_this()));
}

template<>
void expr_template<mod>::accept(visitor v) const
{
    v->visit(downcast<mod>(shared_from_this()));
}

template<>
void expr_template<greater>::accept(visitor v) const
{
    v->visit(downcast<greater>(shared_from_this()));
}

template<>
void expr_template<greater_equal>::accept(visitor v) const
{
    v->visit(downcast<greater_equal>(shared_from_this()));
}

template<>
void expr_template<less>::accept(visitor v) const
{
    v->visit(downcast<less>(shared_from_this()));
}

template<>
void expr_template<logical_and>::accept(visitor v) const
{
    v->visit(downcast<logical_and>(shared_from_this()));
}

template<>
void expr_template<reduce>::accept(visitor v) const
{
    v->visit(downcast<reduce>(shared_from_this()));
}

} // namespace hlir
} // namespace tcc
