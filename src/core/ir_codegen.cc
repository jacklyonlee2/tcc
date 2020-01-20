#include "tcc/core/ir_codegen.h"

namespace tcc {
namespace core {

std::string ir_flatten::apply(symbol_map symbols, expr ir)
{
    std::shared_ptr<ir_flatten> v(new ir_flatten);
    v->symbols = symbols;

    ir->accept(v);
    return v->get_symbol(ir);
}

std::string ir_flatten::get_symbol(expr e)
{
    if (symbols.find(e) == symbols.end())
    {
        if (e->type == expr_type::cnst && e->shape.empty())
        {
            switch (e->dtype)
            {
                case data_type::FP32:
                    symbols.insert(
                        { e,
                          std::to_string(
                              downcast<cnst>(e)->to_scalar<float>()) +
                              "f" });
                    break;
                case data_type::INT64:
                    symbols.insert(
                        { e,
                          std::to_string(
                              downcast<cnst>(e)->to_scalar<int64_t>()) +
                              "l" });
                    break;
                default:
                    tcc_error("unsupported dtype.");
            }
        }
        else
        {
            ir_visitor::visit(e);
            tcc_assert_has_key(symbols, e);
            return symbols.at(e);
        }
    }
    return symbols.at(e);
}

std::string ir_flatten::get_symbols(exprs es)
{
    std::string symbols;
    for (unsigned i = 0; i < es.size() - 1; i++)
    {
        symbols += get_symbol(es[i]) + ",";
    }
    symbols += get_symbol(es[es.size() - 1]);
    return symbols;
}

void ir_flatten::add_symbol(expr e, std::string symbol)
{
    tcc_assert_no_key(symbols, e);
    symbols.insert({ e, symbol });
}

void ir_flatten::visit(index_expr e)
{
    add_symbol(e, get_symbol(e->x) + "[" + get_symbols(e->indices) + "]");
}

void ir_flatten::visit(exp_expr e)
{
    add_symbol(e, "exp(" + get_symbol(e->x) + ")");
}

void ir_flatten::visit(sqrt_expr e)
{
    add_symbol(e, "sqrt(" + get_symbol(e->x) + ")");
}

void ir_flatten::visit(add_expr e)
{
    add_symbol(e, "(" + get_symbol(e->x) + "+" + get_symbol(e->y) + ")");
}

void ir_flatten::visit(sub_expr e)
{
    add_symbol(e, "(" + get_symbol(e->x) + "-" + get_symbol(e->y) + ")");
}

void ir_flatten::visit(mul_expr e)
{
    add_symbol(e, "(" + get_symbol(e->x) + "*" + get_symbol(e->y) + ")");
}

void ir_flatten::visit(div_expr e)
{
    add_symbol(e, "(" + get_symbol(e->x) + "/" + get_symbol(e->y) + ")");
}

void ir_flatten::visit(mod_expr e)
{
    add_symbol(e, "(" + get_symbol(e->x) + "%" + get_symbol(e->y) + ")");
}

void ir_flatten::visit(greater_expr e)
{
    add_symbol(e, "(" + get_symbol(e->x) + ">" + get_symbol(e->y) + ")");
}

void ir_flatten::visit(greater_equal_expr e)
{
    add_symbol(e, "(" + get_symbol(e->x) + ">=" + get_symbol(e->y) + ")");
}

void ir_flatten::visit(less_expr e)
{
    add_symbol(e, "(" + get_symbol(e->x) + "<" + get_symbol(e->y) + ")");
}

void ir_flatten::visit(logical_and_expr e)
{
    add_symbol(e, "(" + get_symbol(e->x) + "&&" + get_symbol(e->y) + ")");
}

void ir_codegen::apply(std::string output_path, expr ir)
{
    std::shared_ptr<ir_codegen> v(new ir_codegen);

    std::ofstream file(output_path, std::ios::trunc);
    tcc_assert(file, "failed to open file at \"" + output_path + "\".");

    v->source = "void func() {\n";
    ir->accept(v);
    v->source += "}";

    file << v->source;
    file.close();
}

std::string ir_codegen::add_variable(expr e)
{
    static unsigned counter = 1;
    tcc_assert_no_key(symbols, e);
    symbols.insert({ e, "v" + std::to_string(counter++) });
    return symbols.at(e);
}

void ir_codegen::alias_variable(expr e, expr ae)
{
    tcc_assert_has_key(symbols, ae);
    tcc_assert_no_key(symbols, e);
    symbols.insert({ e, symbols.at(ae) });
}

void ir_codegen::nest(exprs ranges,
                      expr e,
                      std::function<std::string(symbol_map)> generate_stmt)
{
    static exprs prev_ranges;

    tcc_assert(!ranges.empty(), "loop ranges is empty.");

    exprs idx_ranges;
    unsigned idx = 1;
    unsigned matched_idx = 0;

    /* create loop index symbols. */
    for (expr r : ranges)
    {
        if (downcast<range>(r)->bound == 1)
        {
            symbols.insert({ r, "1" });
        }
        else
        {
            /* find continuously matched index for loop coalescing. */
            if (prev_ranges.size() >= idx &&
                downcast<range>(r)->bound ==
                    downcast<range>(prev_ranges[idx - 1])->bound &&
                idx == matched_idx + 1)
            {
                matched_idx++;
            }
            symbols.insert({ r, "i" + std::to_string(idx++) });
            idx_ranges.push_back(r);
        }
    }

    /* remove closing braces for coalescable ranges. */
    tcc_assert(source.size() >= 2 * matched_idx, "source size is too small.");
    source.resize(source.size() - 2 * matched_idx);

    /* add opening braces for new ranges. */
    for (unsigned i = matched_idx; i < idx_ranges.size(); i++)
    {
        std::string var = symbols.at(idx_ranges[i]);
        source += "for (int " + var + "=0;" + var + "<" +
                  std::to_string(downcast<range>(idx_ranges[i])->bound) + ";" +
                  var + "++) {\n";
    }

    /* add nested statements. */
    source += add_variable(e) + "=" + generate_stmt(symbols) + ";";

    /* add closing braces. */
    for (expr r : idx_ranges)
    {
        source += "}\n";
    }

    prev_ranges = idx_ranges;
}

void ir_codegen::visit(var_expr e)
{
    add_variable(e);
}

void ir_codegen::visit(cnst_expr e)
{
    add_variable(e);
}

void ir_codegen::visit(index_expr e)
{
    ir_visitor::visit(e->x);

    nest(e->ranges, e, [&](symbol_map sym) -> std::string {
        return ir_flatten::apply(sym, e);
    });
}

void ir_codegen::visit(select_expr e)
{
    if (e->t->type == expr_type::index)
    {
        ir_visitor::visit(downcast<index>(e->t)->x);
    }

    if (e->f->type == expr_type::index)
    {
        ir_visitor::visit(downcast<index>(e->f)->x);
    }

    nest(e->ranges, e, [&](symbol_map sym) -> std::string {
        return ir_flatten::apply(sym, e->cond) + "?" +
               ir_flatten::apply(sym, e->t) + ":" +
               ir_flatten::apply(sym, e->f);
    });
}

void ir_codegen::visit(reshape_expr e)
{
    ir_visitor::visit(e->x);
    alias_variable(e, e->x);
}

void ir_codegen::visit(exp_expr e)
{
    ir_visitor::visit(e->x);
    add_variable(e);
}

void ir_codegen::visit(sqrt_expr e)
{
    ir_visitor::visit(e->x);
    add_variable(e);
}

void ir_codegen::visit(add_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
    add_variable(e);
}

void ir_codegen::visit(sub_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
    add_variable(e);
}

void ir_codegen::visit(mul_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
    add_variable(e);
}

void ir_codegen::visit(div_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
    add_variable(e);
}

void ir_codegen::visit(mod_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
    add_variable(e);
}

void ir_codegen::visit(greater_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
    add_variable(e);
}

void ir_codegen::visit(greater_equal_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
    add_variable(e);
}

void ir_codegen::visit(less_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
    add_variable(e);
}

void ir_codegen::visit(logical_and_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);
    add_variable(e);
}

void ir_codegen::visit(reduce_expr e)
{
    ir_visitor::visit(e->x);
    add_variable(e);
}

} // namespace core
} // namespace tcc
