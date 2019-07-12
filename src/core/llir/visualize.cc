#include "tcc/core/llir/visualize.h"

#include <fstream>

namespace tcc {
namespace core {

void LLIRVisualizer::write(std::string output_path) {
    std::ofstream file(output_path, std::ios::trunc);
    CHECK(file) << "Failed to open file at '" << output_path << "'.";
    /* Write DOT graph to stream. */
    file << "digraph D {\n" << stream.str() << "}";
    file.close();
}

void LLIRVisualizer::add_node(Expr expr, std::string name) {
    /* Assign name to unvisited node. Add node to stream. */
    static unsigned int count = 0;
    if (node_name_map.find(expr) == node_name_map.end()) {
        node_name_map.insert({expr, name + "-" + std::to_string(count)});
        count++;

        /* Add DOT node to stream. */
        std::string node_name = node_name_map.at(expr);
        stream << "\t\"" << node_name << "\" ";
        stream << "[shape=box, style=filled, penwidth=0, fillcolor=lightgrey]";
        stream << "\n";
    }
}

void LLIRVisualizer::add_edge(Expr src, Expr dst) {
    CHECK_KEY_IN_MAP(src, node_name_map) <<
        "Source node is unvisited.";
    CHECK_KEY_IN_MAP(dst, node_name_map) <<
        "Destination node is unvisited.";

    /* Add DOT edge to stream. */
    std::string src_name = node_name_map.at(src);
    std::string dst_name = node_name_map.at(dst);
    std::vector<long> src_shape = src->data_desc.get_shape();
    stream << "\t\"" << src_name << "\" -> \"" << dst_name;
    stream << "\" [label=\" " << src_shape << " \"]\n";
}

/* Overloaded LLIR Expr visitors. */

void LLIRVisualizer::visit(const expr::VarPtr expr) {
    add_node(expr, "Var");
}

void LLIRVisualizer::visit(const expr::ConstPtr expr) {
    add_node(expr, "Const");
}

void LLIRVisualizer::visit(const expr::RangePtr expr) {
    add_node(expr, "Range");
}

void LLIRVisualizer::visit(const expr::IndexPtr expr) {
    add_node(expr, "Index");

    for (Expr index : expr->indices) {
        recurse(index);

        add_edge(index, expr);
    }

    recurse(expr->tensor);

    add_edge(expr->tensor, expr);
}

void LLIRVisualizer::visit(const expr::ExpPtr expr) {
    add_node(expr, "Exp");

    recurse(expr->x);

    add_edge(expr->x, expr);
}

void LLIRVisualizer::visit(const expr::SqrtPtr expr) {
    add_node(expr, "Sqrt");

    recurse(expr->x);

    add_edge(expr->x, expr);
}

void LLIRVisualizer::visit(const expr::AddPtr expr) {
    add_node(expr, "Add");

    recurse(expr->x);
    recurse(expr->y);

    add_edge(expr->x, expr);
    add_edge(expr->y, expr);
}

void LLIRVisualizer::visit(const expr::SubPtr expr) {
    add_node(expr, "Sub");

    recurse(expr->x);
    recurse(expr->y);

    add_edge(expr->x, expr);
    add_edge(expr->y, expr);
}

void LLIRVisualizer::visit(const expr::MulPtr expr) {
    add_node(expr, "Mul");

    recurse(expr->x);
    recurse(expr->y);

    add_edge(expr->x, expr);
    add_edge(expr->y, expr);
}

void LLIRVisualizer::visit(const expr::DivPtr expr) {
    add_node(expr, "Div");

    recurse(expr->x);
    recurse(expr->y);

    add_edge(expr->x, expr);
    add_edge(expr->y, expr);
}

void LLIRVisualizer::visit(const expr::ModPtr expr) {
    add_node(expr, "Mod");

    recurse(expr->x);
    recurse(expr->y);

    add_edge(expr->x, expr);
    add_edge(expr->y, expr);
}

void LLIRVisualizer::visit(const expr::GreaterPtr expr) {
    add_node(expr, "Greater");

    recurse(expr->x);
    recurse(expr->y);

    add_edge(expr->x, expr);
    add_edge(expr->y, expr);
}

void LLIRVisualizer::visit(const expr::GreaterEqualPtr expr) {
    add_node(expr, "GreaterEqual");

    recurse(expr->x);
    recurse(expr->y);

    add_edge(expr->x, expr);
    add_edge(expr->y, expr);
}

void LLIRVisualizer::visit(const expr::LessPtr expr) {
    add_node(expr, "Less");

    recurse(expr->x);
    recurse(expr->y);

    add_edge(expr->x, expr);
    add_edge(expr->y, expr);
}

void LLIRVisualizer::visit(const expr::AndPtr expr) {
    add_node(expr, "And");

    recurse(expr->x);
    recurse(expr->y);

    add_edge(expr->x, expr);
    add_edge(expr->y, expr);
}

void LLIRVisualizer::visit(const expr::SelectPtr expr) {
    add_node(expr, "Select");

    recurse(expr->condition);
    recurse(expr->t);
    recurse(expr->f);

    add_edge(expr->condition, expr);
    add_edge(expr->t, expr);
    add_edge(expr->f, expr);
}

void LLIRVisualizer::visit(const expr::ReducePtr expr) {
    switch (expr->reduce_type) {
        case ReduceType::SUM:
            add_node(expr, "Reduce:Sum");
            break;
        case ReduceType::AVG:
            add_node(expr, "Reduce:AVG");
            break;
        case ReduceType::MAX:
            add_node(expr, "Reduce:MAX");
            break;
        default:
            LOG(FATAL) << "Unknown reduce type.";
    }

    for (Expr axis : expr->reduce_axes) {
        recurse(axis);

        add_edge(axis, expr);
    }

    recurse(expr->input);

    add_edge(expr->input, expr);
}

} // namespace core
} // namespace tcc
