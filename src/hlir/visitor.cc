#include "tcc/hlir/visitor.h"

namespace tcc {
namespace hlir {

void visitor_base::visit(expr e)
{
    if (visited.find(e) == visited.end())
    {
        visited.insert(e);
        e->accept(shared_from_this());
    }
}

void visitor_base::visit(exprs es)
{
    for (expr e : es)
    {
        visit(e);
    }
}

void visitor_base::visit(var_expr) {}

void visitor_base::visit(cnst_expr) {}

void visitor_base::visit(range_expr) {}

void visitor_base::visit(index_expr e)
{
    visit(e->ranges);
    visit(e->x);
    visit(e->indices);
}

void visitor_base::visit(select_expr e)
{
    visit(e->ranges);
    visit(e->condition);
    visit(e->t);
    visit(e->f);
}

void visitor_base::visit(reshape_expr e)
{
    visit(e->x);
}

void visitor_base::visit(exp_expr e)
{
    visit(e->x);
}

void visitor_base::visit(sqrt_expr e)
{
    visit(e->x);
}

void visitor_base::visit(add_expr e)
{
    visit(e->x);
    visit(e->y);
}

void visitor_base::visit(sub_expr e)
{
    visit(e->x);
    visit(e->y);
}

void visitor_base::visit(mul_expr e)
{
    visit(e->x);
    visit(e->y);
}

void visitor_base::visit(div_expr e)
{
    visit(e->x);
    visit(e->y);
}

void visitor_base::visit(mod_expr e)
{
    visit(e->x);
    visit(e->y);
}

void visitor_base::visit(greater_expr e)
{
    visit(e->x);
    visit(e->y);
}

void visitor_base::visit(greater_equal_expr e)
{
    visit(e->x);
    visit(e->y);
}

void visitor_base::visit(less_expr e)
{
    visit(e->x);
    visit(e->y);
}

void visitor_base::visit(logical_and_expr e)
{
    visit(e->x);
    visit(e->y);
}

void visitor_base::visit(reduce_expr e)
{
    visit(e->x);
}

} // namespace hlir
} // namespace tcc
