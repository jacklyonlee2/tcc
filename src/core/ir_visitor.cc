#include "tcc/core/ir_visitor.h"

namespace tcc {

void ir_visitor::visit(expr e)
{
    tcc_assert_not_null(e);
    if (visited.find(e) == visited.end())
    {
        visited.insert(e);
        e->accept(shared_from_this());
    }
}

void ir_visitor::visit(var_expr) {}

void ir_visitor::visit(cnst_expr) {}

void ir_visitor::visit(range_expr) {}

void ir_visitor::visit(index_expr e)
{
    visit(e->x);
    for (expr index : e->indices)
    {
        visit(index);
    }
}

void ir_visitor::visit(select_expr e)
{
    visit(e->cond);
    visit(e->t);
    visit(e->f);
}

void ir_visitor::visit(reshape_expr e)
{
    visit(e->x);
}

void ir_visitor::visit(reduce_expr e)
{
    visit(e->x);
}

void ir_visitor::visit(unary_expr e)
{
    visit(e->x);
}

void ir_visitor::visit(binary_expr e)
{
    visit(e->x);
    visit(e->y);
}

} // namespace tcc
