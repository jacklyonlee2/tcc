#include "tcc/core/ir_printer.h"
#include "tcc/common/util.h"

namespace tcc {

void ir_printer::apply(std::string output_path, expr ir)
{
    std::shared_ptr<ir_printer> v(new ir_printer);
    v->file = std::ofstream(output_path, std::ios::trunc);
    tcc_assert(v->file, "failed to open file at \"" + output_path + "\".");

    v->file << "digraph core {\n";
    v->file << "\tnode [shape=record]\n";
    ir->accept(v);
    v->file << "}";
    v->file.close();
}

void ir_printer::print_node(expr e,
                            std::string label,
                            std::vector<std::pair<expr, std::string>> inputs)
{
    file << "\t\"" << std::to_string(e.get()) << "\" ";
    file << "[label=\"{";

    if (!inputs.empty())
    {
        file << "{";
        for (unsigned i = 0; i < inputs.size(); i++)
        {
            file << "<f" << std::to_string(inputs[i].first.get()) << "> "
                 << inputs[i].second << ((i < inputs.size() - 1) ? "|" : "");
        }
        file << "}|";
    }

    file << label << "|" << std::to_string(e->dtype);

    if (!e->shape.empty())
    {
        file << "|(";
        for (unsigned i = 0; i < e->shape.size() - 1; i++)
        {
            file << e->shape[i] << ",";
        }
        file << e->shape[e->shape.size() - 1] << ")";
    }

    file << "}\"];\n";
}

void ir_printer::print_edge(expr src, expr dst)
{
    file << "\t\"" << std::to_string(src.get()) << "\" -> \""
         << std::to_string(dst.get()) << "\":f" << std::to_string(src.get())
         << ";\n";
}

void ir_printer::visit(var_expr e)
{
    print_node(e, "var");
}

void ir_printer::visit(cnst_expr e)
{
    if (e->shape.empty())
    {
        switch (e->dtype)
        {
            case datatype::FP32:
                print_node(e, std::to_string(e->to_scalar<float>()));
                break;
            case datatype::INT64:
                print_node(e, std::to_string(e->to_scalar<int64_t>()));
                break;
            default:
                print_node(e, "cnst");
        }
    }
    else
    {
        print_node(e, "cnst");
    }
}

void ir_printer::visit(range_expr e)
{
    print_node(e, "0:" + std::to_string(e->bound));
}

void ir_printer::visit(index_expr e)
{
    std::vector<std::pair<expr, std::string>> inputs({ { e->x, "x" } });
    for (unsigned i = 0; i < e->indices.size(); i++)
    {
        inputs.push_back({ e->indices[i], "i" + std::to_string(i) });
    }

    print_node(e, "index", inputs);

    ir_visitor::visit(e->x);
    ir_visitor::visit(e->indices);

    print_edge(e->x, e);

    for (unsigned i = 0; i < e->indices.size(); i++)
    {
        print_edge(e->indices[i], e);
    }
}

void ir_printer::visit(select_expr e)
{
    print_node(
        e, "select", { { e->cond, "cond" }, { e->t, "t" }, { e->f, "f" } });

    ir_visitor::visit(e->cond);
    ir_visitor::visit(e->t);
    ir_visitor::visit(e->f);

    print_edge(e->cond, e);
    print_edge(e->t, e);
    print_edge(e->f, e);
}

void ir_printer::visit(reshape_expr e)
{
    print_node(e, "reshape", { { e->x, "x" } });

    ir_visitor::visit(e->x);

    print_edge(e->x, e);
}

void ir_printer::visit(reduce_expr e)
{
    print_node(e, "reduce", { { e->x, "x" } });

    ir_visitor::visit(e->x);

    print_edge(e->x, e);
}

void ir_printer::visit(unary_expr e)
{
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

    print_node(e, expr_symbol, { { e->x, "x" } });

    ir_visitor::visit(e->x);

    print_edge(e->x, e);
}

void ir_printer::visit(binary_expr e)
{
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
                return "//";
            case binary::type::mod:
                return "%";
            default:
                tcc_error("unknown binary type.");
        }
    }());

    print_node(e, expr_symbol, { { e->x, "x" }, { e->y, "y" } });

    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);

    print_edge(e->x, e);
    print_edge(e->y, e);
}

void ir_printer::visit(logical_expr e)
{
    std::string expr_symbol = ([&]() {
        switch (e->logical_type)
        {
            case logical::type::greater:
                return "\\>";
            case logical::type::greater_equal:
                return "\\>=";
            case logical::type::less:
                return "\\<";
            case logical::type::and_:
                return "&&";
            default:
                tcc_error("unknown logical type.");
        }
    }());

    print_node(e, "\\>", { { e->x, "x" }, { e->y, "y" } });

    ir_visitor::visit(e->x);
    ir_visitor::visit(e->y);

    print_edge(e->x, e);
    print_edge(e->y, e);
}

} // namespace tcc
