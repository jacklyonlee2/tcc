#include "tcc/hlir/visitor.h"

namespace tcc {
namespace hlir {

void
visitor::recurse(expr e)
{
    if (visited.find(e) == visited.end())
    {
        visited.insert(e);
        e->accept(this);
    }
}

void
visitor::visit(var_expr)
{}

void
visitor::visit(cnst_expr)
{}

} // namespace hlir
} // namespace tcc
