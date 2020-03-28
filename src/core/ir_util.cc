#include "tcc/core/ir_util.h"
#include <algorithm>

namespace tcc {

expr exp(expr e)
{
    return unary::make(unary::type::exp, e);
}

expr operator+(expr lhs, expr rhs)
{
    return binary::make(binary::type::add, lhs, rhs);
}

expr operator-(expr lhs, expr rhs)
{
    return binary::make(binary::type::sub, lhs, rhs);
}

expr operator*(expr lhs, expr rhs)
{
    return binary::make(binary::type::mul, lhs, rhs);
}

expr operator/(expr lhs, expr rhs)
{
    return binary::make(binary::type::div, lhs, rhs);
}

expr operator%(expr lhs, expr rhs)
{
    return binary::make(binary::type::mod, lhs, rhs);
}

expr operator&&(expr lhs, expr rhs)
{
    return binary::make(binary::type::logical_and, lhs, rhs);
}

expr operator>(expr lhs, expr rhs)
{
    return binary::make(binary::type::greater, lhs, rhs);
}

expr operator>=(expr lhs, expr rhs)
{
    return binary::make(binary::type::greater_eq, lhs, rhs);
}

expr operator<(expr lhs, expr rhs)
{
    return binary::make(binary::type::less, lhs, rhs);
}

std::string to_string(exprtype et)
{
    switch (et)
    {
        case exprtype::var:
            return "var";
        case exprtype::cnst:
            return "cnst";
        case exprtype::range:
            return "range";
        case exprtype::index:
            return "index";
        case exprtype::select:
            return "select";
        case exprtype::reshape:
            return "reshape";
        case exprtype::reduce:
            return "reduce";
        case exprtype::unary:
            return "unary";
        case exprtype::binary:
            return "binary";
        default:
            tcc_error("unknown exprtype.");
    }
}

exprs to_ranges(dimensions shape)
{
    exprs ranges;
    std::transform(shape.begin(),
                   shape.end(),
                   std::back_inserter(ranges),
                   [](dimension dim) -> expr {
                       tcc_assert(dim > 0, "dimension of shape is negative.");
                       return dim == 1 ? cnst::make(0ll) : range::make(dim);
                   });
    return ranges;
}

dimensions to_shape(exprs ranges)
{
    dimensions shape;
    std::transform(ranges.begin(),
                   ranges.end(),
                   std::back_inserter(shape),
                   [](expr e) -> dimension {
                       return e->type == exprtype::cnst
                                  ? 1l
                                  : downcast<range>(e)->bound;
                   });
    return shape;
}

bool index_validator::apply(exprs ranges, exprs indices)
{
    std::shared_ptr<index_validator> v(new index_validator);
    v->valid_ranges = std::unordered_set<expr>(ranges.begin(), ranges.end());

    for (expr idx : indices)
    {
        idx->accept(v);
    }

    return !v->found_invalid_range;
}

void index_validator::visit(range_expr e)
{
    if (valid_ranges.find(e) == valid_ranges.end())
    {
        found_invalid_range = false;
    }
}

} // namespace tcc
