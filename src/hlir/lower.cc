#include "tcc/hlir/lower.h"

namespace tcc {
namespace hlir {

llir::blk lower::apply(expr hlir)
{
    return nullptr;
}

void lower::visit(var_expr) {}

void lower::visit(cnst_expr) {}

void lower::visit(range_expr) {}

void lower::visit(index_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->indices);
}

void lower::visit(select_expr e)
{
    visitor_base::visit(e->cond);
    visitor_base::visit(e->t);
    visitor_base::visit(e->f);
}

void lower::visit(reshape_expr e)
{
    visitor_base::visit(e->x);
}

void lower::visit(exp_expr e)
{
    visitor_base::visit(e->x);
}

void lower::visit(sqrt_expr e)
{
    visitor_base::visit(e->x);
}

void lower::visit(add_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void lower::visit(sub_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void lower::visit(mul_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void lower::visit(div_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void lower::visit(mod_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void lower::visit(greater_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void lower::visit(greater_equal_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void lower::visit(less_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void lower::visit(logical_and_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void lower::visit(reduce_expr e)
{
    visitor_base::visit(e->x);
}

} // namespace hlir
} // namespace tcc
