#include "tcc/core/ir_codegen.h"
#include "tcc/core/ir_dep_analysis.h"
#include "tcc/core/ir_util.h"
#include <fstream>

namespace tcc {

static exprs squeeze_ranges(exprs ranges)
{
    exprs squeezed_ranges;
    std::remove_copy_if(
        ranges.begin(),
        ranges.end(),
        std::back_inserter(squeezed_ranges),
        [](expr e) -> bool { return e->type == exprtype::cnst; });
    return squeezed_ranges;
}

static unsigned match_ranges(exprs old, exprs current)
{
    unsigned matched_dims = 0;
    for (expr dim : current)
    {
        if (matched_dims >= old.size() ||
            downcast<range>(dim)->bound !=
                downcast<range>(old[matched_dims])->bound)
        {
            break;
        }
        matched_dims++;
    }
    return matched_dims;
}

void ir_codegen::apply(std::string output_path, expr ir)
{
    /* dependency analysis. */
    ir_dep_analysis_result result = ir_dep_analysis::apply(ir);

    /* initialize and apply codegen visitor */
    std::shared_ptr<ir_codegen> v(new ir_codegen);
    v->reused = result.reused;
    v->output = ir;
    ir->accept(v);

    /* open output file and write generated code to file. */
    std::ofstream file(output_path, std::ios::trunc);
    tcc_assert(file, "failed to open file at " + output_path + ".");
    file << "void func() {" + v->body + "}";
    file.close();
}

void ir_codegen::add_local_symbol(expr e, std::string symbol)
{
    tcc_assert(local_symbols.find(e) == local_symbols.end() ||
                   local_symbols.at(e) == symbol,
               "a different symbol for e already exists.");
    local_symbols.insert({ e, symbol });
}

std::string ir_codegen::add_global_symbol(expr e, std::string symbol)
{
    if (symbol.empty())
    {
        tcc_assert_no_key(global_symbols, e);
        static unsigned vcount = 1;
        global_symbols.insert({ e, "v" + std::to_string(vcount++) });
        return global_symbols.at(e);
    }
    else
    {
        global_symbols.insert({ e, symbol });
        return symbol;
    }
}

std::string ir_codegen::get_indices(exprs ranges, exprs indices)
{
    // TODO flatten indices.
    tcc_assert(!ranges.empty(), "ranges is empty.");
    if (indices.empty())
    {
        return "[...]";
    }
    else
    {
        return "[...]";
    }
}

std::string ir_codegen::get_symbol(expr e)
{
    std::string symbol;

    if (local_symbols.find(e) != local_symbols.end())
    {
        symbol = local_symbols.at(e);
        local_symbols.erase(e);
    }
    else if (global_symbols.find(e) != global_symbols.end())
    {
        symbol = global_symbols.at(e);
    }
    else if (ir_visitor::visited.find(e) == ir_visitor::visited.end())
    {
        ir_visitor::visit(e);
    }
    else
    {
        tcc_error("symbol for e is not found.");
    }

    return symbol;
}

void ir_codegen::nest(exprs ranges,
                      expr e,
                      std::function<std::string()> generate_stmt,
                      bool force_append)
{
    std::function<void(unsigned, exprs)> update_local =
        [&](unsigned matched_dims, exprs new_ranges) {
            static unsigned icount = 1;
            for (unsigned i = 0; i < new_ranges.size(); i++)
            {
                add_global_symbol(new_ranges[i],
                                  i < matched_dims
                                      ? get_symbol(local_ranges[i])
                                      : "i" + std::to_string(icount++));
            }
            local_ranges = new_ranges;
        };

    std::function<void(unsigned)> close_loop = [&](unsigned matched_dims) {
        for (unsigned i = matched_dims; i < local_ranges.size(); i++)
        {
            body += "}\n";
        }
    };

    std::function<void(unsigned)> open_loop = [&](unsigned matched_dims) {
        for (unsigned i = matched_dims; i < local_ranges.size(); i++)
        {
            std::string index_symbol = get_symbol(local_ranges[i]);
            std::string index_bound =
                std::to_string(downcast<range>(local_ranges[i])->bound);
            body += "for (int64_t " + index_symbol + " = 0;" + index_symbol +
                    " < " + index_bound + "; " + index_symbol + "++ ) {\n";
        }
    };

    std::function<void(unsigned)> flush_local_and_close_loop =
        [&](unsigned matched_dims) {
            for (auto it = local_symbols.cbegin(); it != local_symbols.cend();)
            {
                if (!it->first->shape.empty())
                {
                    body += add_global_symbol(it->first) +
                            get_indices(local_ranges) + " = " + it->second +
                            ";";
                    it = local_symbols.erase(it);
                }
                else
                {
                    it++;
                }
            }

            close_loop(matched_dims);

            /* flush scalar exprs. */
            if (matched_dims == 0)
            {
                for (auto it = local_symbols.cbegin();
                     it != local_symbols.cend();
                     it = local_symbols.erase(it))
                {
                    if (it->first == output)
                    {
                        body += add_global_symbol(it->first) + " = " +
                                it->second + ";";
                    }
                }
            }
        };

    if (e->shape.empty())
    {
        if (generate_stmt != nullptr)
        {
            if (force_append)
            {
                body += generate_stmt();
            }
            else
            {
                add_local_symbol(e, generate_stmt());
            }
        }
    }
    else // TODO Force global @ index, coalesce for in-place computes only
    {
        ranges = squeeze_ranges(ranges);
        unsigned matched_dims = match_ranges(local_ranges, ranges);

        if (matched_dims < local_ranges.size())
        {
            flush_local_and_close_loop(matched_dims);
        }
        else
        {
            close_loop(matched_dims);
        }

        update_local(matched_dims, ranges);
        open_loop(matched_dims);

        if (generate_stmt != nullptr)
        {
            if (force_append)
            {
                body += generate_stmt();
            }
            else if (reused.find(e) != reused.end())
            {
                body += add_global_symbol(e) + get_indices(local_ranges) +
                        " = " + generate_stmt() + ";";
            }
            else
            {
                add_local_symbol(e, generate_stmt());
            }
        }
    }

    if (e == output)
    {
        flush_local_and_close_loop(0);
    }
}

void ir_codegen::visit(var_expr e)
{
    add_global_symbol(e);
}

void ir_codegen::visit(cnst_expr e)
{

    if (e->shape.empty())
    {
        switch (e->dtype)
        {
            case datatype::FP32:
                add_global_symbol(e,
                                  std::to_string(e->to_scalar<float>()) + "f");
                break;
            case datatype::INT64:
                add_global_symbol(
                    e, std::to_string(e->to_scalar<int64_t>()) + "l");
                break;
            default:
                tcc_error("unsupported dtype.");
        }
    }
    else
    {
        add_global_symbol(e);
    }
}

void ir_codegen::visit(index_expr e)
{
    ir_visitor::visit(e->x);

    nest(e->ranges, e, [&]() {
        return get_symbol(e->x) + get_indices(e->ranges, e->indices);
    });
}

void ir_codegen::visit(select_expr e)
{
    ir_visitor::visit(e->t);
    ir_visitor::visit(e->f);

    nest(e->ranges, e, [&]() {
        std::string t_symbol =
            e->t->shape.empty()
                ? get_symbol(e->t)
                : (get_symbol(e->t) + get_indices(to_ranges(e->t->shape)));
        std::string f_symbol =
            e->f->shape.empty()
                ? get_symbol(e->f)
                : (get_symbol(e->f) + get_indices(to_ranges(e->f->shape)));

        return "(" + get_symbol(e->cond) + "?" + t_symbol + ":" + f_symbol +
               ")";
    });
}

void ir_codegen::visit(reshape_expr e)
{
    ir_visitor::visit(e->x);

    // TODO Force global + inplace assignments
    /* alias symbol with e->x. */
    add_global_symbol(e, get_symbol(e->x));

    nest(to_ranges(e->shape), e);
}

void ir_codegen::visit(reduce_expr e)
{
    ir_visitor::visit(e->x);

    exprs unreduced_ranges = to_ranges(e->x->shape);
    exprs reduced_ranges; // TODO compute reduced range

    nest(unreduced_ranges,
         e,
         [&]() {
             std::string e_symbol =
                 e->shape.empty()
                     ? add_global_symbol(e)
                     : (add_global_symbol(e) +
                        get_indices(unreduced_ranges, reduced_ranges));
             std::string x_symbol =
                 get_symbol(e->x) + get_indices(unreduced_ranges);

             switch (e->reduce_type)
             {
                 case reduce::type::avg:
                     return e_symbol + "+=" + x_symbol + "/" +
                            std::to_string(e->reduce_size) + "f;";
                 case reduce::type::max:
                     return e_symbol + "=" + x_symbol + ">" + e_symbol + "?" +
                            x_symbol + ":" + e_symbol + ";";
                 case reduce::type::sum:
                     return e_symbol + "+=" + x_symbol + ";";
                 default:
                     tcc_error("unknown reduce type");
             }
         },
         true);

    nest(reduced_ranges, e);
}

void ir_codegen::visit(unary_expr e)
{
    ir_visitor::visit(e->x);

    std::string expr_symbol = ([&]() {
        switch (e->unary_type)
        {
            case unary::type::exp:
                return "exp";
            case unary::type::sqrt:
                return "sqrt";
            default:
                tcc_error("unknown unary type.");
        }
    }());

    exprs e_ranges = to_ranges(e->shape);

    nest(e_ranges, e, [&]() {
        std::string x_symbol = e->shape.empty()
                                   ? get_symbol(e->x)
                                   : get_symbol(e->x) + get_indices(e_ranges);

        return expr_symbol + "(" + x_symbol + ")";
    });
}

void ir_codegen::visit(binary_expr e)
{
    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);

    std::string expr_symbol = ([&]() {
        switch (e->binary_type)
        {
            case binary::type::add:
                return "+";
            case binary::type::sub:
                return "-";
            case binary::type::mul:
                return "*";
            case binary::type::div:
                return "/";
            case binary::type::mod:
                return "%";
            case binary::type::logical_and:
                return "&&";
            case binary::type::greater:
                return ">";
            case binary::type::greater_eq:
                return ">=";
            case binary::type::less:
                return "<";
            default:
                tcc_error("unknown binary type.");
        }
    }());

    exprs e_ranges = to_ranges(e->shape);

    nest(e_ranges, e, [&]() {
        std::string x_symbol = e->x->shape.empty()
                                   ? get_symbol(e->x)
                                   : get_symbol(e->x) + get_indices(e_ranges);
        std::string y_symbol = e->y->shape.empty()
                                   ? get_symbol(e->y)
                                   : get_symbol(e->y) + get_indices(e_ranges);

        return "(" + x_symbol + expr_symbol + y_symbol + ")";
    });
}

} // namespace tcc
