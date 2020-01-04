#include "tcc/affn/ir_lower.h"

namespace tcc {
namespace affn {

lang::prim ir_lower::apply(expr ir)
{
    return nullptr;
}

void ir_lower::visit(var_expr) {}

void ir_lower::visit(cnst_expr) {}

void ir_lower::visit(range_expr) {}

void ir_lower::visit(index_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->indices);
}

void ir_lower::visit(select_expr e)
{
    ir_visitor::visit(e->cond);
    ir_visitor::visit(e->t);
    ir_visitor::visit(e->f);
}

void ir_lower::visit(reshape_expr e)
{
    ir_visitor::visit(e->x);
}

void ir_lower::visit(exp_expr e)
{
    ir_visitor::visit(e->x);
}

void ir_lower::visit(sqrt_expr e)
{
    ir_visitor::visit(e->x);
}

void ir_lower::visit(add_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
}

void ir_lower::visit(sub_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
}

void ir_lower::visit(mul_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
}

void ir_lower::visit(div_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
}

void ir_lower::visit(mod_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
}

void ir_lower::visit(greater_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
}

void ir_lower::visit(greater_equal_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
}

void ir_lower::visit(less_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
}

void ir_lower::visit(logical_and_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
}

void ir_lower::visit(reduce_expr e)
{
    ir_visitor::visit(e->x);
}

} // namespace affn
} // namespace tcc
