#include "tcc/core/ir_dep_analysis.h"

namespace tcc {

ir_dep_analysis_result ir_dep_analysis::apply(expr ir)
{
    std::shared_ptr<ir_dep_analysis> v(new ir_dep_analysis);
    ir->accept(v);
    return v->result;
}

void ir_dep_analysis::analyze(expr e)
{
    if (ir_visitor::visited.count(e) && !e->shape.empty())
    {
        result.reused_non_scalars.insert(e);
    }
    ir_visitor::visit(e);
}

void ir_dep_analysis::visit(var_expr e)
{
    result.inputs.push_back(e);
}

void ir_dep_analysis::visit(index_expr e)
{
    analyze(e->x);
}

void ir_dep_analysis::visit(select_expr e)
{
    analyze(e->cond);
    analyze(e->t);
    analyze(e->f);
}

void ir_dep_analysis::visit(reshape_expr e)
{
    analyze(e->x);
}

void ir_dep_analysis::visit(reduce_expr e)
{
    analyze(e->x);
}

void ir_dep_analysis::visit(unary_expr e)
{
    analyze(e->x);
}

void ir_dep_analysis::visit(binary_expr e)
{
    analyze(e->x);
    analyze(e->y);
}

} // namespace tcc
