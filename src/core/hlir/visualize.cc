#include "tcc/core/hlir/visualize.h"

#include <fstream>

namespace tcc {
namespace core {

void HLIRVisualizer::write(std::string output_path) {
    std::ofstream file(output_path, std::ios::trunc);
    CHECK(file) << "Failed to open file at '" << output_path << "'.";
    /* Write DOT graph to stream. */
    file << "digraph D {\n" << stream.str() << "}";
    file.close();
}

void HLIRVisualizer::add_node(Op op, std::string name, bool storage_node) {
    /* Assign name to unvisited node. Add node to stream. */
    static unsigned int count = 0;
    if (node_name_map.find(op) == node_name_map.end()) {
        node_name_map.insert({op, name + "-" + std::to_string(count)});
        count++;

        /* Add DOT node to stream. */
        std::string node_name = node_name_map.at(op);
        stream << "\t\"" << node_name << "\" ";
        stream << (storage_node ?
            "[shape=box, style=filled, penwidth=0, fillcolor=lightgrey]" :
            "[shape=box, style=filled, fillcolor=black, fontcolor=white]");
        stream << "\n";
    }
}

void HLIRVisualizer::add_edge(Op src, Op dst, std::string label) {
    CHECK_KEY_IN_MAP(src, node_name_map) <<
        "Source node is unvisited.";
    CHECK_KEY_IN_MAP(dst, node_name_map) <<
        "Destination node is unvisited.";

    /* Add DOT edge to stream. */
    std::string src_name = node_name_map.at(src);
    std::string dst_name = node_name_map.at(dst);
    stream << "\t\"" << src_name << "\" -> \"" << dst_name;
    stream << "\" [label=\" " << label << " \"]\n";
}

/* Overloaded HLIR Op visitors. */

void HLIRVisualizer::visit(const op::PlaceholderPtr op) {
    add_node(op, "Placeholder", true);
}

void HLIRVisualizer::visit(const op::ConstantPtr op) {
    add_node(op, "Constant", true);
}

void HLIRVisualizer::visit(const op::IntermediatePtr op) {
    add_node(op, "Intermediate", true);

    recurse(op->prev_op);

    CHECK_NOT_EXPIRED(op->prev_op);
    add_edge(op->prev_op.lock(), op, "output");
}

void HLIRVisualizer::visit(const op::AddPtr op) {
    add_node(op, "Add");

    recurse(op->x);
    recurse(op->y);

    add_edge(op->x, op, "x");
    add_edge(op->y, op, "y");
}

void HLIRVisualizer::visit(const op::AvgPoolPtr op) {
    add_node(op, "AvgPool");

    recurse(op->value);

    add_edge(op->value, op, "value");
}

void HLIRVisualizer::visit(const op::BiasAddPtr op) {
    add_node(op, "BiasAdd");

    recurse(op->input);
    recurse(op->bias);

    add_edge(op->input, op, "input");
    add_edge(op->bias, op, "bias");
}

void HLIRVisualizer::visit(const op::Conv2DPtr op) {
    add_node(op, "Conv2D");

    recurse(op->input);
    recurse(op->filter);

    add_edge(op->input, op, "input");
    add_edge(op->filter, op, "filter");
}

void HLIRVisualizer::visit(const op::DepthwiseConv2dNativePtr op) {
    add_node(op, "DepthwiseConv2dNative");

    recurse(op->input);
    recurse(op->filter);

    add_edge(op->input, op, "input");
    add_edge(op->filter, op, "filter");
}

void HLIRVisualizer::visit(const op::FusedBatchNormPtr op) {
    add_node(op, "FusedBatchNorm");

    recurse(op->x);
    recurse(op->scale);
    recurse(op->offset);
    recurse(op->mean);
    recurse(op->variance);

    add_edge(op->x, op, "x");
    add_edge(op->scale, op, "scale");
    add_edge(op->offset, op, "offset");
    add_edge(op->mean, op, "mean");
    add_edge(op->variance, op, "variance");
}

void HLIRVisualizer::visit(const op::Relu6Ptr op) {
    add_node(op, "Relu6");

    recurse(op->features);

    add_edge(op->features, op, "features");
}

void HLIRVisualizer::visit(const op::ReshapePtr op) {
    add_node(op, "Reshape");

    recurse(op->tensor);
    recurse(op->shape);

    add_edge(op->tensor, op, "tensor");
    add_edge(op->shape, op, "shape");
}

void HLIRVisualizer::visit(const op::ShapePtr op) {
    add_node(op, "Shape");

    recurse(op->input);

    add_edge(op->input, op, "input");
}

void HLIRVisualizer::visit(const op::SoftmaxPtr op) {
    add_node(op, "Softmax");

    recurse(op->logits);

    add_edge(op->logits, op, "logits");
}

void HLIRVisualizer::visit(const op::SqueezePtr op) {
    add_node(op, "Squeeze");

    recurse(op->input);

    add_edge(op->input, op, "input");
}

} // namespace core
} // namespace tcc
