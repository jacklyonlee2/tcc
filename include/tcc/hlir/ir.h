#ifndef TCC_HLIR_IR_H
#define TCC_HLIR_IR_H

#include "tcc/common/datatype.h"
#include "tcc/common/logging.h"
#include "tcc/common/util.h"
#include <functional>
#include <memory>
#include <numeric>
#include <unordered_set>
#include <vector>

namespace tcc {
namespace hlir {

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

/* forward declare hlir visitor class. */
struct visitor_base;
typedef std::shared_ptr<visitor_base> visitor;

/* expr_base is the base class all hlir primitives derive from.
 * expr_base stores the expr_type of the derived class, the
 * shape of its output and "accept" virtual method to support
 * visitor design pattern. */
struct expr_base
{
    expr_base(expr_type t)
        : type(t)
    {}

    virtual void accept(visitor v) const = 0;

    expr_type type;
    mutable data_type dtype;
    mutable dimensions shape;
};

/* expr_template is a template class
 * used to declare hlir primitives. */
template<typename T>
struct expr_template
    : expr_base
    , std::enable_shared_from_this<expr_template<T>>
{
    expr_template()
        : expr_base(T::etype)
    {}

    void accept(visitor v) const override;
};

/* aliases and syntax sugars for the hlir api. */
typedef std::shared_ptr<const expr_base> expr;
typedef std::vector<expr> exprs;

/* hlir primitive declarations.
 * when declaring a new hlir primitive:
 * * add the respective expr_type to the expr_type enum class;
 * * declare the struct for the respective hlir primitive deriving
 *   for the expr_template class;
 * * declare and implement "make" method used for initialization
 *   in place of the default constructor;
 * * declare "etype" field initialized to the respective expr_type;
 * * add a new type alias for pointers to the respective hlir primitive;
 * * add a new visit method to the hlir visitor class and all of
 *   its derived classes for ther respective hlir primitive.
 * * override the respective "accept" method of expr_template. */

struct var : expr_template<var>
{
    static expr make(data_type, dimensions);

    static const expr_type etype = expr_type::var;
};

struct cnst : expr_template<cnst>
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

struct range : expr_template<range>
{
    dimension bound;

    static expr make(dimension);

    static const expr_type etype = expr_type::range;
};

struct index : expr_template<index>
{
    exprs ranges;
    expr x;
    exprs indices;

    static expr make(exprs, expr, exprs);

    static const expr_type etype = expr_type::index;
};

struct select : expr_template<select>
{
    exprs ranges;
    expr cond;
    expr t;
    expr f;

    static expr make(exprs, expr, expr, expr);

    static const expr_type etype = expr_type::select;
};

struct reshape : expr_template<reshape>
{
    expr x;

    static expr make(dimensions, expr);

    static const expr_type etype = expr_type::reshape;
};

struct exp : expr_template<exp>
{
    expr x;

    static expr make(expr);

    static const expr_type etype = expr_type::exp;
};

struct sqrt : expr_template<sqrt>
{
    expr x;

    static expr make(expr);

    static const expr_type etype = expr_type::sqrt;
};

struct add : expr_template<add>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::add;
};

struct sub : expr_template<sub>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::sub;
};

struct mul : expr_template<mul>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::mul;
};

struct div : expr_template<div>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::div;
};

struct mod : expr_template<mod>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::mod;
};

struct greater : expr_template<greater>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::greater;
};

struct greater_equal : expr_template<greater_equal>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::greater_equal;
};

struct less : expr_template<less>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::less;
};

struct logical_and : expr_template<logical_and>
{
    expr x;
    expr y;

    static expr make(expr, expr);

    static const expr_type etype = expr_type::logical_and;
};

struct reduce : expr_template<reduce>
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

/* aliases for pointers to hlir primitives. */

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

/* overload arithmetic and logical operators for expr. */

expr operator+(expr lhs, expr rhs);
expr operator-(expr lhs, expr rhs);
expr operator*(expr lhs, expr rhs);
expr operator/(expr lhs, expr rhs);
expr operator%(expr lhs, expr rhs);
expr operator>(expr lhs, expr rhs);
expr operator>=(expr lhs, expr rhs);
expr operator<(expr lhs, expr rhs);
expr operator&&(expr lhs, expr rhs);

/* downcast casts expr to derived expr. */
template<typename T>
std::shared_ptr<const T> downcast(expr e)
{
    tcc_assert(e && e->type == T::etype, "illegal downcast.");
    return std::static_pointer_cast<const T>(e);
}

/* construct ranges from shape. */
inline exprs to_ranges(dimensions shape)
{
    exprs ranges;

    for (dimension dim : shape)
    {
        ranges.push_back(range::make(dim));
    }

    return ranges;
}

/* construct shape from ranges. */
inline dimensions to_shape(exprs ranges)
{
    dimensions shape;

    for (expr e : ranges)
    {
        shape.push_back(downcast<range>(e)->bound);
    }

    return shape;
}

} // namespace hlir
} // namespace tcc

#endif // TCC_HLIR_IR_H
