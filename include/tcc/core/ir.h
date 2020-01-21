#ifndef TCC_CORE_IR_H
#define TCC_CORE_IR_H

#include "tcc/common/datatype.h"
#include "tcc/common/logging.h"
#include "tcc/common/util.h"
#include <memory>
#include <numeric>
#include <unordered_set>
#include <vector>

namespace tcc {

enum class exprtype
{
    var,
    cnst,
    range,
    index,
    select,
    reshape,
    reduce,
    unary,
    binary,
    logical
};

struct ir_visitor;

struct abstract_expr
{
    abstract_expr(exprtype t)
        : type(t)
    {}

    virtual void accept(std::shared_ptr<ir_visitor> v) const = 0;

    exprtype type;
    mutable datatype dtype;
    mutable dimensions shape;
};

template<typename T>
struct base_expr
    : abstract_expr
    , std::enable_shared_from_this<base_expr<T>>
{
    base_expr()
        : abstract_expr(T::expr_type)
    {}

    void accept(std::shared_ptr<ir_visitor> v) const override;
};

typedef std::shared_ptr<const abstract_expr> expr;
typedef std::vector<expr> exprs;

struct var : base_expr<var>
{
    static expr make(datatype, dimensions);

    static const exprtype expr_type = exprtype::var;
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

    template<typename T>
    static expr make(T data)
    {
        std::shared_ptr<cnst> e(new cnst);
        e->data = scalar_serialize<T>(data);
        e->dtype = to_datatype<T>();
        return e;
    }

    template<typename T>
    static expr make(std::vector<T> data)
    {
        std::shared_ptr<cnst> e(new cnst);
        e->data = vector_serialize<T>(data);
        e->dtype = to_datatype<T>();
        e->shape = dimensions({ static_cast<dimension>(data.size()) });
        return e;
    }

    static expr make(std::string, datatype, dimensions);

    static const exprtype expr_type = exprtype::cnst;
};

struct range : base_expr<range>
{
    dimension bound;

    static expr make(dimension);

    static const exprtype expr_type = exprtype::range;
};

struct index : base_expr<index>
{
    exprs ranges;
    expr x;
    exprs indices;

    static expr make(exprs, expr, exprs);

    static const exprtype expr_type = exprtype::index;
};

struct select : base_expr<select>
{
    exprs ranges;
    expr cond;
    expr t;
    expr f;

    static expr make(exprs, expr, expr, expr);

    static const exprtype expr_type = exprtype::select;
};

struct reshape : base_expr<reshape>
{
    expr x;

    static expr make(dimensions, expr);

    static const exprtype expr_type = exprtype::reshape;
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

    static const exprtype expr_type = exprtype::reduce;
};

struct unary : base_expr<unary>
{
    enum class type
    {
        exp,
        sqrt
    };

    type unary_type;
    expr x;

    static expr make(type, expr);

    static const exprtype expr_type = exprtype::unary;
};

struct binary : base_expr<binary>
{
    enum class type
    {
        add,
        sub,
        mul,
        div,
        mod
    };

    type binary_type;
    expr x;
    expr y;

    static expr make(type, expr, expr);

    static const exprtype expr_type = exprtype::binary;
};

struct logical : base_expr<logical>
{
    enum class type
    {
        greater,
        greater_equal,
        less,
        and_
    };

    type logical_type;
    expr x;
    expr y;

    static expr make(type, expr, expr);

    static const exprtype expr_type = exprtype::logical;
};

typedef std::shared_ptr<const var> var_expr;
typedef std::shared_ptr<const cnst> cnst_expr;
typedef std::shared_ptr<const range> range_expr;
typedef std::shared_ptr<const index> index_expr;
typedef std::shared_ptr<const select> select_expr;
typedef std::shared_ptr<const reshape> reshape_expr;
typedef std::shared_ptr<const reduce> reduce_expr;
typedef std::shared_ptr<const unary> unary_expr;
typedef std::shared_ptr<const binary> binary_expr;
typedef std::shared_ptr<const logical> logical_expr;

} // namespace tcc

#endif // TCC_CORE_IR_H
