#ifndef TCC_AFFN_IR_H
#define TCC_AFFN_IR_H

#include "tcc/common/datatype.h"
#include "tcc/common/logging.h"
#include "tcc/common/util.h"
#include <memory>
#include <numeric>
#include <unordered_set>
#include <vector>

namespace tcc {
namespace affn {

enum class expr_type
{
    var,
    cnst,
    range,
    index,
    select,
    reshape,
    exp,
    sqrt,
    add,
    sub,
    mul,
    div,
    mod,
    greater,
    greater_equal,
    less,
    logical_and,
    reduce
};

struct ir_visitor;

/* abstract base class for affn exprs. */
struct abstract_expr
{
    abstract_expr(expr_type t)
        : type(t)
    {}

    virtual void accept(std::shared_ptr<ir_visitor> v) const = 0;

    expr_type type;
    mutable data_type dtype;
    mutable dimensions shape;
};

/* base class for affn exprs. */
template<typename T>
struct base_expr
    : abstract_expr
    , std::enable_shared_from_this<base_expr<T>>
{
    base_expr()
        : abstract_expr(T::etype)
    {}

    void accept(std::shared_ptr<ir_visitor> v) const override;
};

typedef std::shared_ptr<const abstract_expr> expr;
typedef std::vector<expr> exprs;

struct var : base_expr<var>
{
    static expr make(data_type, dimensions);

    static const expr_type etype = expr_type::var;
};

struct cnst : base_expr<cnst>
{
    std::string data;

    template<typename T>
    T to_scalar() const
    {
        tcc_assert(shape.empty(), "can not convert vector to scalar.");
        return scalar_deserialize<T>(data);
    }

    template<typename T>
    std::vector<T> to_vector() const
    {
        tcc_assert(!shape.empty(), "can not convert scalar to vector.");
        return vector_deserialize<T>(
            data,
            std::accumulate(
                shape.begin(), shape.end(), 1, std::multiplies<dimension>()));
    }

    static expr make(std::string, data_type, dimensions);
    static expr make(float);
    static expr make(dimension);
    static expr make(dimensions);

    static const expr_type etype = expr_type::cnst;
};

struct range : base_expr<range>
{
    dimension bound;

    static expr make(dimension);

    static const expr_type etype = expr_type::range;
};

struct index : base_expr<index>
{
    exprs ranges;
    expr x;
    exprs indices;

    static expr make(exprs, expr, exprs);

    static const expr_type etype = expr_type::index;
};

struct select : base_expr<select>
{
    exprs ranges;
    expr cond;
    expr t;
    expr f;

    static expr make(exprs, expr, expr, expr);

    static const expr_type etype = expr_type::select;
};

struct reshape : base_expr<reshape>
{
    expr x;

    static expr make(dimensions, expr);

    static const expr_type etype = expr_type::reshape;
};

struct exp : base_expr<exp>
{
    expr x;

    static expr make(expr);

    static const expr_type etype = expr_type::exp;
};

struct sqrt : base_expr<sqrt>
{
    expr x;

    static expr make(expr);

    static const expr_type etype = expr_type::sqrt;
};

struct add : base_expr<add>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::add;
};

struct sub : base_expr<sub>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::sub;
};

struct mul : base_expr<mul>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::mul;
};

struct div : base_expr<div>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::div;
};

struct mod : base_expr<mod>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::mod;
};

struct greater : base_expr<greater>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::greater;
};

struct greater_equal : base_expr<greater_equal>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::greater_equal;
};

struct less : base_expr<less>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::less;
};

struct logical_and : base_expr<logical_and>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::logical_and;
};

struct reduce : base_expr<reduce>
{
    enum class type
    {
        avg,
        max,
        sum
    };

    type reduce_type;
    std::unordered_set<unsigned> reduce_dims;
    expr x;

    static expr make(type, std::unordered_set<unsigned>, expr);

    static const expr_type etype = expr_type::reduce;
};

typedef std::shared_ptr<const var> var_expr;
typedef std::shared_ptr<const cnst> cnst_expr;
typedef std::shared_ptr<const range> range_expr;
typedef std::shared_ptr<const index> index_expr;
typedef std::shared_ptr<const select> select_expr;
typedef std::shared_ptr<const reshape> reshape_expr;
typedef std::shared_ptr<const exp> exp_expr;
typedef std::shared_ptr<const sqrt> sqrt_expr;
typedef std::shared_ptr<const add> add_expr;
typedef std::shared_ptr<const sub> sub_expr;
typedef std::shared_ptr<const mul> mul_expr;
typedef std::shared_ptr<const div> div_expr;
typedef std::shared_ptr<const mod> mod_expr;
typedef std::shared_ptr<const greater> greater_expr;
typedef std::shared_ptr<const greater_equal> greater_equal_expr;
typedef std::shared_ptr<const less> less_expr;
typedef std::shared_ptr<const logical_and> logical_and_expr;
typedef std::shared_ptr<const reduce> reduce_expr;

expr operator+(expr lhs, expr rhs);
expr operator-(expr lhs, expr rhs);
expr operator*(expr lhs, expr rhs);
expr operator/(expr lhs, expr rhs);
expr operator%(expr lhs, expr rhs);
expr operator>(expr lhs, expr rhs);
expr operator>=(expr lhs, expr rhs);
expr operator<(expr lhs, expr rhs);
expr operator&&(expr lhs, expr rhs);

/* downcast casts expr to affn expr. */
template<typename T>
std::shared_ptr<const T> downcast(expr e)
{
    tcc_assert(e && e->type == T::etype, "illegal downcast.");
    return std::static_pointer_cast<const T>(e);
}

/* to_ranges construct ranges from shape. */
inline exprs to_ranges(dimensions shape)
{
    exprs ranges;

    for (dimension dim : shape)
    {
        ranges.push_back(range::make(dim));
    }

    return ranges;
}

/* to_shape construct shape from ranges. */
inline dimensions to_shape(exprs ranges)
{
    dimensions shape;

    for (expr e : ranges)
    {
        shape.push_back(downcast<range>(e)->bound);
    }

    return shape;
}

} // namespace affn
} // namespace tcc

#endif // TCC_AFFN_IR_H
