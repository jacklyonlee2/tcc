#include "tcc/hlir/dot_printer.h"
#include <fstream>
#include <sstream>

namespace tcc {
namespace hlir {

inline std::string to_address(const void* p)
{
    std::stringstream ss;
    ss << p;
    return ss.str();
}

void dot_printer::apply(std::string output_path, expr hlir)
{
    std::shared_ptr<dot_printer> v(new dot_printer);

    static_cast<visitor>(v)->visit(hlir);

    std::ofstream file(output_path, std::ios::trunc);
    tcc_assert(file, "failed to open file at \"" + output_path + "\".");
    file << v->ir.str();
    file.close();
}

void dot_printer::visit(var_expr e)
{
    ir.add_node(e, "var");
}

void dot_printer::visit(cnst_expr e)
{
    if (e->shape.empty())
    {
        switch (e->dtype)
        {
            case data_type::FP32:
                ir.add_node(e, std::to_string(e->to_scalar<float>()));
                break;
            case data_type::INT64:
                ir.add_node(e, std::to_string(e->to_scalar<int64_t>()));
                break;
            default:
                ir.add_node(e, "cnst");
        }
    }
    else
    {
        ir.add_node(e, "cnst");
    }
}

void dot_printer::visit(range_expr e)
{
    ir.add_node(e, "range");
}

void dot_printer::visit(index_expr e)
{
    std::vector<std::string> inputs({ "x" });
    for (unsigned i = 0; i < e->indices.size(); i++)
    {
        inputs.push_back("i" + std::to_string(i));
    }

    ir.add_node(e, "index", inputs);

    visitor_base::visit(e->x);
    visitor_base::visit(e->indices);

    ir.add_edge(e->x, e, "x");

    for (unsigned i = 0; i < e->indices.size(); i++)
    {
        ir.add_edge(e->indices[i], e, "i" + std::to_string(i));
    }
}

void dot_printer::visit(select_expr e)
{
    ir.add_node(e, "select", { "cond", "t", "f" });

    visitor_base::visit(e->cond);
    visitor_base::visit(e->t);
    visitor_base::visit(e->f);

    ir.add_edge(e->cond, e, "cond");
    ir.add_edge(e->t, e, "t");
    ir.add_edge(e->f, e, "f");
}

void dot_printer::visit(reshape_expr e)
{
    ir.add_node(e, "reshape", { "x" });

    visitor_base::visit(e->x);

    ir.add_edge(e->x, e, "x");
}

void dot_printer::visit(exp_expr e)
{
    ir.add_node(e, "exp", { "x" });

    visitor_base::visit(e->x);

    ir.add_edge(e->x, e, "x");
}

void dot_printer::visit(sqrt_expr e)
{
    ir.add_node(e, "sqrt", { "x" });

    visitor_base::visit(e->x);

    ir.add_edge(e->x, e, "x");
}

void dot_printer::visit(add_expr e)
{
    ir.add_node(e, "+", { "x", "y" });

    visitor_base::visit(e->x);
    visitor_base::visit(e->y);

    ir.add_edge(e->x, e, "x");
    ir.add_edge(e->y, e, "y");
}

void dot_printer::visit(sub_expr e)
{
    ir.add_node(e, "-", { "x", "y" });

    visitor_base::visit(e->x);
    visitor_base::visit(e->y);

    ir.add_edge(e->x, e, "x");
    ir.add_edge(e->y, e, "y");
}

void dot_printer::visit(mul_expr e)
{
    ir.add_node(e, "*", { "x", "y" });

    visitor_base::visit(e->x);
    visitor_base::visit(e->y);

    ir.add_edge(e->x, e, "x");
    ir.add_edge(e->y, e, "y");
}

void dot_printer::visit(div_expr e)
{
    ir.add_node(e, "/", { "x", "y" });

    visitor_base::visit(e->x);
    visitor_base::visit(e->y);

    ir.add_edge(e->x, e, "x");
    ir.add_edge(e->y, e, "y");
}

void dot_printer::visit(mod_expr e)
{
    ir.add_node(e, "%", { "x", "y" });

    visitor_base::visit(e->x);
    visitor_base::visit(e->y);

    ir.add_edge(e->x, e, "x");
    ir.add_edge(e->y, e, "y");
}

void dot_printer::visit(greater_expr e)
{
    ir.add_node(e, "\\>", { "x", "y" });

    visitor_base::visit(e->x);
    visitor_base::visit(e->y);

    ir.add_edge(e->x, e, "x");
    ir.add_edge(e->y, e, "y");
}

void dot_printer::visit(greater_equal_expr e)
{
    ir.add_node(e, "\\>=", { "x", "y" });

    visitor_base::visit(e->x);
    visitor_base::visit(e->y);

    ir.add_edge(e->x, e, "x");
    ir.add_edge(e->y, e, "y");
}

void dot_printer::visit(less_expr e)
{
    ir.add_node(e, "\\<", { "x", "y" });

    visitor_base::visit(e->x);
    visitor_base::visit(e->y);

    ir.add_edge(e->x, e, "x");
    ir.add_edge(e->y, e, "y");
}

void dot_printer::visit(logical_and_expr e)
{
    ir.add_node(e, "and", { "x", "y" });

    visitor_base::visit(e->x);
    visitor_base::visit(e->y);

    ir.add_edge(e->x, e, "x");
    ir.add_edge(e->y, e, "y");
}

void dot_printer::visit(reduce_expr e)
{
    ir.add_node(e, "reduce", { "x" });

    visitor_base::visit(e->x);

    ir.add_edge(e->x, e, "x");
}

void dot_printer::graph::add_node(expr e,
                                  std::string etype,
                                  std::vector<std::string> inputs)
{
    tcc_assert_no_key(nodes, e);
    nodes.insert({ e, { e, etype, e->dtype, e->shape, inputs } });
}

void dot_printer::graph::add_edge(expr src, expr dst, std::string dst_input)
{
    tcc_assert_has_key(nodes, src);
    tcc_assert_has_key(nodes, dst);
    edges.push_back({ src, dst, nodes.at(dst).get_input_label(dst_input) });
}

std::string dot_printer::graph::str() const
{
    std::string dot_str = "digraph hlir {\n";
    dot_str += "\tnode [shape=record]\n";
    for (std::pair<expr, node> n : nodes)
    {
        dot_str += "\t" + n.second.str() + "\n";
    }
    for (edge e : edges)
    {
        dot_str += "\t" + e.str() + "\n";
    }
    return dot_str + "}";
}

dot_printer::graph::node::node(expr e,
                               std::string etype,
                               data_type dtype,
                               dimensions shape,
                               std::vector<std::string> inputs)
{
    this->e = to_address(e.get());
    this->etype = etype;

    switch (dtype)
    {
        case data_type::BOOL:
            this->dtype = "BOOL";
            break;
        case data_type::FP32:
            this->dtype = "FP32";
            break;
        case data_type::INT32:
            this->dtype = "INT32";
            break;
        case data_type::INT64:
            this->dtype = "INT64";
            break;
        default:
            tcc_error("unknown data type.");
    }

    if (!shape.empty())
    {
        this->shape = "(";
        for (unsigned i = 0; i < shape.size() - 1; i++)
        {
            this->shape += std::to_string(shape[i]) + ",";
        }
        this->shape += std::to_string(shape[shape.size() - 1]) + ")";
    }

    this->inputs = inputs;

    for (unsigned i = 0; i < inputs.size(); i++)
    {
        this->input_labels.insert({ inputs[i], "f" + std::to_string(i) });
    }
}

std::string dot_printer::graph::node::get_input_label(std::string input) const
{
    tcc_assert_has_key(this->input_labels, input);
    return this->input_labels.at(input);
}

std::string dot_printer::graph::node::str() const
{
    std::string dot_str = "\"" + this->e + "\" ";
    dot_str += "[label=\"{";
    if (!this->inputs.empty())
    {
        dot_str += "{";
        for (unsigned i = 0; i < this->inputs.size(); i++)
        {
            dot_str += "<" + get_input_label(this->inputs[i]) + "> " +
                       this->inputs[i] +
                       ((i < this->inputs.size() - 1) ? "|" : "");
        }
        dot_str += "}|";
    }
    dot_str += this->etype + "|" + this->dtype +
               (this->shape.empty() ? "" : "|") + this->shape + "}\"];";
    return dot_str;
}

dot_printer::graph::edge::edge(expr src, expr dst, std::string dst_input_label)
{
    this->src = to_address(src.get());
    this->dst = to_address(dst.get());
    this->dst_input_label = dst_input_label;
}

std::string dot_printer::graph::edge::str() const
{
    return "\"" + this->src + "\" -> \"" + this->dst +
           "\":" + this->dst_input_label + ";";
}

} // namespace hlir
} // namespace tcc
