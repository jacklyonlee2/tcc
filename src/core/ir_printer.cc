#include "tcc/core/ir_printer.h"
#include <sstream>

namespace tcc {

static std::string ptr_to_addr(const void* p)
{
    std::stringstream ss;
    ss << p;
    return ss.str();
}

void ir_printer::apply(const std::string target_name, expr ir)
{
    std::string dot_path = target_name + "/" + target_name + ".dot";
    std::ofstream file = std::ofstream(dot_path, std::ios::trunc);
    tcc_assert(file, "failed to open file at " + dot_path + ".");

    std::shared_ptr<ir_printer> v(new ir_printer);
    v->file = std::move(file);
    v->file << "digraph core {\n"
            << "\tnode [shape=record]\n";
    ir->accept(v);
    v->file << "}";
    v->file.close();
}

void ir_printer::print_node(expr e,
                            std::string label,
                            std::vector<std::pair<expr, std::string>> inputs)
{
    file << "\t\"" << ptr_to_addr(e.get()) << "\" ";
    file << "[label=\"{";

    if (!inputs.empty())
    {
        file << "{";
        for (unsigned i = 0; i < inputs.size(); i++)
        {
            file << "<f" << ptr_to_addr(inputs[i].first.get()) << "> "
                 << inputs[i].second << ((i < inputs.size() - 1) ? "|" : "");
        }
        file << "}|";
    }

    file << label << "|" << ([&]() {
        switch (e->dtype)
        {
            case datatype::BOOL:
                return "BOOL";
            case datatype::FP32:
                return "FP32";
            case datatype::INT32:
                return "INT32";
            case datatype::INT64:
                return "INT64";
            default:
                tcc_error("unknown data type.");
        }
    }());

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
    file << "\t\"" << ptr_to_addr(src.get()) << "\" -> \""
         << ptr_to_addr(dst.get()) << "\":f" << ptr_to_addr(src.get()) << ";\n";
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
                return print_node(e, std::to_string(e->to_scalar<float>()));
            case datatype::INT64:
                return print_node(e, std::to_string(e->to_scalar<int64_t>()));
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
    for (expr index : e->indices)
    {
        ir_visitor::visit(index);
    }

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
    std::string expr_symbol = ([&]() {
        switch (e->reduce_type)
        {
            case reduce::type::avg:
                return "reduce avg";
            case reduce::type::max:
                return "reduce max";
            case reduce::type::sum:
                return "reduce sum";
            default:
                tcc_error("unknown reduce type.");
        }
    }());

    print_node(e, expr_symbol, { { e->x, "x" } });

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
                return "/";
            case binary::type::mod:
                return "%";
            case binary::type::logical_and:
                return "&&";
            case binary::type::greater:
                return "\\>";
            case binary::type::greater_eq:
                return "\\>=";
            case binary::type::less:
                return "\\<";
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

} // namespace tcc
