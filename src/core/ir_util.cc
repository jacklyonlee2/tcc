#include "tcc/core/ir_util.h"

namespace tcc {

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

expr exp(expr e)
{
    return unary::make(unary::type::exp, e);
}

expr sqrt(expr e)
{
    return unary::make(unary::type::sqrt, e);
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

} // namespace tcc
