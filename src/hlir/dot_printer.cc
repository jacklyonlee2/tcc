#include "tcc/hlir/dot_printer.h"

namespace tcc {
namespace hlir {

void dot_printer::apply(std::string output_path, expr hlir) {}

void dot_printer::visit(var_expr) {}

void dot_printer::visit(cnst_expr) {}

void dot_printer::visit(range_expr) {}

void dot_printer::visit(index_expr e)
{
    visitor_base::visit(e->ranges);
    visitor_base::visit(e->x);
    visitor_base::visit(e->indices);
}

void dot_printer::visit(select_expr e)
{
    visitor_base::visit(e->ranges);
    visitor_base::visit(e->condition);
    visitor_base::visit(e->t);
    visitor_base::visit(e->f);
}

void dot_printer::visit(reshape_expr e)
{
    visitor_base::visit(e->x);
}

void dot_printer::visit(exp_expr e)
{
    visitor_base::visit(e->x);
}

void dot_printer::visit(sqrt_expr e)
{
    visitor_base::visit(e->x);
}

void dot_printer::visit(add_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void dot_printer::visit(sub_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void dot_printer::visit(mul_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void dot_printer::visit(div_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void dot_printer::visit(mod_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void dot_printer::visit(greater_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void dot_printer::visit(greater_equal_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void dot_printer::visit(less_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void dot_printer::visit(logical_and_expr e)
{
    visitor_base::visit(e->x);
    visitor_base::visit(e->y);
}

void dot_printer::visit(reduce_expr e)
{
    visitor_base::visit(e->x);
}

} // namespace hlir
} // namespace tcc
