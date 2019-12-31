#include "tcc/hlir/visitor.h"

namespace tcc {
namespace hlir {

void visitor::recurse(expr e)
{
    if (visited.find(e) == visited.end())
    {
        visited.insert(e);
        e->accept(this);
    }
}

void visitor::visit(var_expr) {}

void visitor::visit(cnst_expr) {}

void visitor::visit(range_expr) {}

void visitor::visit(index_expr) {}

void visitor::visit(select_expr) {}

void visitor::visit(exp_expr) {}

void visitor::visit(sqrt_expr) {}

void visitor::visit(add_expr) {}

void visitor::visit(sub_expr) {}

void visitor::visit(mul_expr) {}

void visitor::visit(div_expr) {}

void visitor::visit(mod_expr) {}

void visitor::visit(greater_expr) {}

void visitor::visit(greater_equal_expr) {}

void visitor::visit(less_expr) {}

void visitor::visit(logical_and_expr) {}

void visitor::visit(reduce_expr) {}

} // namespace hlir
} // namespace tcc
