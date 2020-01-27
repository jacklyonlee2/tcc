#include "tcc/core/ir_codegen.h"
#include "tcc/core/ir_dep_analysis.h"
#include "tcc/core/ir_util.h"
#include <fstream>

namespace tcc {

/* remove all cnst dimensions from ranges. */
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

/* count coalesable dimensions of given ranges. */
static unsigned match_ranges(exprs old, exprs current)
{
    unsigned matched_dims = 0;
    for (expr dim : current)
    {
        if (matched_dims < old.size())
        {
            tcc_assert(dim->type == exprtype::range &&
                           old[matched_dims]->type == exprtype::range,
                       "non-squeezed dimension is found.");
            if (downcast<range>(dim)->bound !=
                downcast<range>(old[matched_dims])->bound)
            {
                break;
            }
            matched_dims++;
        }
    }
    return matched_dims;
}

/* convert tcc datatype to c datatype string. */
static std::string datatype_to_ctype_str(datatype dtype)
{
    switch (dtype)
    {
        case datatype::FP32:
            return "float";
        case datatype::INT64:
            return "int";
        case datatype::INT32:
            return "int";
        default:
            tcc_error("unsupported datatype.");
    }
}

/* convert shape to array size string. */
static std::string shape_to_size_str(dimensions shape)
{
    return shape.empty() ? ""
                         : ("[" +
                            std::to_string(std::accumulate(
                                shape.begin(),
                                shape.end(),
                                1l,
                                [](dimension size, dimension dim) {
                                    return size * dim;
                                })) +
                            "]");
}

/* convert non-scalar cnst to c initializer list. */
template<typename T>
void cnst_to_file(std::ofstream& f, expr e)
{
    f << "{";
    for (T ele : downcast<cnst>(e)->to_vector<T>())
    {
        f << std::to_string(ele) << ",";
    }
    f << "}";
}

void ir_codegen::apply(const std::string target_name, expr ir)
{
    /* dependency analysis. */
    ir_dep_analysis_result result = ir_dep_analysis::apply(ir);

    /* initialize and apply codegen visitor */
    std::shared_ptr<ir_codegen> v(new ir_codegen);
    v->reused_non_scalars = result.reused_non_scalars;
    v->output = ir;
    ir->accept(v);

    /* generate static global variables. */
    std::function<std::string(expr)> generate_var_signature = [&v](expr e) {
        tcc_assert_has_key(v->global_symbols, e);
        return datatype_to_ctype_str(e->dtype) + " " + v->global_symbols.at(e) +
               shape_to_size_str(e->shape);
    };

    /* generate function signature. */
    exprs inouts(result.inputs.begin(), result.inputs.end());
    inouts.push_back(v->output);
    std::function<std::string()> generate_func_signature = [&]() {
        return "void " + target_name + "(" +
               std::accumulate(inouts.begin() + 1,
                               inouts.end(),
                               generate_var_signature(inouts[0]),
                               [&](std::string str, expr e) {
                                   return str + "," + generate_var_signature(e);
                               }) +
               ")";
    };

    /* generate header file. */
    std::string header_path = target_name + "/" + target_name + ".h";
    std::ofstream hfile(header_path, std::ios::trunc);
    tcc_assert(hfile, "failed to open file at " + header_path);

    hfile << "#pragma once\n"
          << "extern " << generate_func_signature() << ";";
    hfile.close();

    /* generate source file. */
    std::string source_path = target_name + "/" + target_name + ".c";
    std::ofstream sfile(source_path, std::ios::trunc);
    tcc_assert(sfile, "failed to open file at " + source_path);

    sfile << "#include <math.h>\n"
          << "#include \"" + target_name + ".h\"\n";

    tcc_assert_has_key(v->global_symbols, v->output);
    for (auto symbol : v->global_symbols)
    {
        if (!(symbol.first->shape.empty() &&
              (symbol.first->type == exprtype::cnst ||
               symbol.first->type == exprtype::range)) &&
            symbol.second != v->global_symbols.at(v->output) &&
            result.inputs.find(symbol.first) == result.inputs.end())
        {
            if (symbol.first->type == exprtype::cnst)
            {
                sfile << "static const " << generate_var_signature(symbol.first)
                      << "=";
                switch (symbol.first->dtype)
                {
                    case datatype::FP32:
                        cnst_to_file<float>(sfile, symbol.first);
                        break;
                    case datatype::INT32:
                        cnst_to_file<int32_t>(sfile, symbol.first);
                        break;
                    default:
                        tcc_error("unsupported datatype.");
                }
                sfile << ";\n";
            }
            else
            {
                sfile << "static " << generate_var_signature(symbol.first)
                      << ";\n";
            }
        }
    }

    sfile << generate_func_signature() << "{" + v->body + "}";
    sfile.close();
}

void ir_codegen::add_local_symbol(expr e, std::string symbol)
{
    tcc_assert(!symbol.empty(), "symbol is empty.");
    tcc_assert(local_symbols.find(e) == local_symbols.end() ||
                   local_symbols.at(e) == symbol,
               "a different symbol for e already exists.");
    local_symbols.insert({ e, symbol });
}

std::string ir_codegen::add_global_symbol(expr e, std::string symbol)
{
    if (symbol.empty())
    {
        static unsigned vcount = 1;
        tcc_assert_no_key(global_symbols, e);
        global_symbols.insert({ e, "v" + std::to_string(vcount++) });
        return global_symbols.at(e);
    }
    else
    {
        global_symbols.insert({ e, symbol });
        return symbol;
    }
}

std::string ir_codegen::get_indices(exprs ranges,
                                    dimensions shape,
                                    exprs indices)
{
    ranges = squeeze_ranges(ranges);
    tcc_assert(!ranges.empty(), "ranges is empty.");

    unsigned matched_dims = match_ranges(local_ranges, ranges);
    tcc_assert(matched_dims == ranges.size(), "unmatched index ranges.");

    /* alias index symbols. */
    for (unsigned i = 0; i < ranges.size(); i++)
    {
        tcc_assert_has_key(global_symbols, local_ranges[i]);
        if (ranges[i] != local_ranges[i])
        {
            add_global_symbol(ranges[i], global_symbols.at(local_ranges[i]));
        }
    }

    shape = indices.empty() ? to_shape(ranges) : shape;
    indices = indices.empty() ? ranges : indices;
    tcc_assert(indices.size() == shape.size(),
               "size of indices does not equal to size of shape.");

    std::string flattened_index;
    dimension index_multiplier = 1;
    for (int i = shape.size() - 1; i >= 0; i--)
    {
        std::string multiplier_symbol =
            index_multiplier == 1 ? ""
                                  : ("*" + std::to_string(index_multiplier));
        std::string remainder_symbol =
            flattened_index.empty() ? "" : ("+" + flattened_index);
        flattened_index = "(" + get_symbol(indices[i]) + multiplier_symbol +
                          ")" + remainder_symbol;

        index_multiplier *= shape[i];
    }

    return "[" + flattened_index + "]";
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
        symbol =
            (e->shape.empty()
                 ? global_symbols.at(e)
                 : (global_symbols.at(e) + get_indices(to_ranges(e->shape))));
    }
    else if (ir_visitor::visited.find(e) == ir_visitor::visited.end())
    {
        ir_visitor::visit(e);
        symbol = get_symbol(e);
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
            body += "for(" + datatype_to_ctype_str(local_ranges[i]->dtype) +
                    " " + index_symbol + "=0;" + index_symbol + "<" +
                    index_bound + ";" + index_symbol + "++){\n";
        }
    };

    std::function<void(unsigned)> flush_local_and_close_loop =
        [&](unsigned matched_dims) {
            for (auto it = local_symbols.cbegin(); it != local_symbols.cend();)
            {
                if (!it->first->shape.empty() || it->first == output)
                {
                    body += add_global_symbol(it->first) +
                            (it->first->shape.empty()
                                 ? ""
                                 : get_indices(to_ranges(it->first->shape)) +
                                       "=" + it->second + ";");
                    it = local_symbols.erase(it);
                }
                else
                {
                    it++;
                }
            }

            close_loop(matched_dims);
        };

    if (!e->shape.empty())
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
    }

    if (generate_stmt != nullptr)
    {
        if (force_append)
        {
            body += generate_stmt();
        }
        else if (reused_non_scalars.find(e) != reused_non_scalars.end())
        {
            body += add_global_symbol(e) + get_indices(ranges) + " = " +
                    generate_stmt() + ";";
        }
        else
        {
            add_local_symbol(e, generate_stmt());
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
                add_global_symbol(e, std::to_string(e->to_scalar<int64_t>()));
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

    nest(e->ranges, e);

    if (local_symbols.find(e->x) != local_symbols.end())
    {
        exprs x_ranges = to_ranges(e->x->shape);
        nest(e->ranges,
             e,
             [&]() {
                 return add_global_symbol(e->x) + get_indices(x_ranges) + "=" +
                        get_symbol(e->x) + ";";
             },
             true);
    }

    tcc_assert_has_key(global_symbols, e->x);
    nest(e->ranges, e, [&]() {
        return global_symbols.at(e->x) +
               get_indices(e->ranges, e->x->shape, e->indices);
    });
}

void ir_codegen::visit(select_expr e)
{
    ir_visitor::visit(e->t);
    ir_visitor::visit(e->f);

    nest(e->ranges, e, [&]() {
        return "(" + get_symbol(e->cond) + "?" + get_symbol(e->t) + ":" +
               get_symbol(e->f) + ")";
    });
}

void ir_codegen::visit(reshape_expr e)
{
    ir_visitor::visit(e->x);

    exprs e_ranges = to_ranges(e->shape);
    nest(e_ranges, e);

    if (global_symbols.find(e->x) != global_symbols.end())
    {
        add_global_symbol(e, global_symbols.at(e->x));
    }
    else if (reused_non_scalars.find(e) != reused_non_scalars.end())
    {
        nest(e_ranges,
             e,
             [&]() {
                 return add_global_symbol(e) + get_indices(e_ranges) + "=" +
                        get_symbol(e->x) + ";";
             },
             true);
    }
    else
    {
        add_local_symbol(e, get_symbol(e->x));
    }
}

void ir_codegen::visit(reduce_expr e)
{
    ir_visitor::visit(e->x);

    exprs unreduced_ranges = to_ranges(e->x->shape);
    exprs reduced_ranges;
    for (unsigned i = 0; i < unreduced_ranges.size(); i++)
    {
        if (e->reduce_dims.find(i) == e->reduce_dims.end())
        {
            reduced_ranges.push_back(unreduced_ranges[i]);
        }
    }

    nest(unreduced_ranges,
         e,
         [&]() {
             std::string x_symbol = get_symbol(e->x);
             std::string e_symbol =
                 e->shape.empty()
                     ? add_global_symbol(e)
                     : add_global_symbol(e) + get_indices(unreduced_ranges,
                                                          e->shape,
                                                          reduced_ranges);

             switch (e->reduce_type)
             {
                 case reduce::type::avg:
                     return e_symbol + "+=" + x_symbol + "/" +
                            std::to_string(e->reduce_size) + ".f;";
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

    nest(to_ranges(e->shape), e, [&]() {
        return expr_symbol + "(" + get_symbol(e->x) + ")";
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

    nest(to_ranges(e->shape), e, [&]() {
        return "(" + get_symbol(e->x) + expr_symbol + get_symbol(e->y) + ")";
    });
}

} // namespace tcc
