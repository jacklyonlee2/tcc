#include "tcc/frontend/parse.h"
#include "graph.pb.h"
#include "tcc/common/logging.h"
#include "tcc/frontend/op.h"
#include <fstream>

namespace tcc {
namespace frontend {

static tensorflow::GraphDef
load_graph(std::string input_path)
{
    std::fstream file;
    file.open(input_path, std::ios::in | std::ios::binary);
    tcc_assert(file,
               "failed to open tensorflow frozen graph at \"" + input_path +
                   "\".");

    tensorflow::GraphDef graph;
    tcc_assert(graph.ParseFromIstream(&file),
               "failed to parse tensorflow frozen graph.");
    file.close();

    return graph;
}

static void
parse_node(tensorflow::NodeDef& node,
           std::unordered_map<std::string, hlir::expr>& parsed_nodes)
{
    if (node.op() == "Placeholder")
    {
    }
    else if (node.op() == "Const")
    {}
    else if (node.op() == "Add")
    {
        tcc_assert(node.input().size() == 2,
                   "incorrect number of inputs for tensorflow op \"" +
                       node.op() + "\".");

        hlir::expr x = parsed_nodes.at(node.input()[0]);
        hlir::expr y = parsed_nodes.at(node.input()[1]);
        hlir::expr z = parse_op_add(x, y);

        tcc_assert_no_key(parsed_nodes, node.name());
        parsed_nodes.insert({ node.name(), z });
    }
    else if (node.op() == "AvgPool")
    {}
    else if (node.op() == "BiasAdd")
    {}
    else if (node.op() == "Conv2D")
    {}
    else if (node.op() == "DepthwiseConv2dNative")
    {}
    else if (node.op() == "FusedBatchNorm")
    {}
    else if (node.op() == "Relu6")
    {}
    else if (node.op() == "Reshape")
    {}
    else if (node.op() == "Shape")
    {}
    else if (node.op() == "Softmax")
    {}
    else if (node.op() == "Squeeze")
    {}
    else
    {
        tcc_error("unsupported tensorflow op \"" + node.op() + "\".");
    }
}

static void
recurse_graph(std::string current_node_name,
              std::unordered_map<std::string, tensorflow::NodeDef>& nodes,
              std::unordered_map<std::string, hlir::expr>& parsed_nodes,
              std::unordered_map<std::string, std::vector<long>>& input_shapes)
{
    tcc_assert_has_key(nodes, current_node_name);
    tensorflow::NodeDef current_node = nodes.at(current_node_name);

    /* recurses the inputs of the current node;
     * reaches base case when there is no inputs to the current node. */
    for (std::string input_node_name : current_node.input())
    {
        recurse_graph(input_node_name, nodes, parsed_nodes, input_shapes);
        tcc_assert_has_key(parsed_nodes, input_node_name);
    }

    parse_node(current_node, parsed_nodes);
}

static hlir::expr
parse_graph(tensorflow::GraphDef& graph,
            std::unordered_map<std::string, std::vector<long>>& input_shapes)
{
    /* collect all tensorflow nodes into hashtable and
     * find output nodes of the tensorflow graph. */
    std::unordered_map<std::string, tensorflow::NodeDef> nodes;
    std::unordered_set<std::string> output_names;

    for (tensorflow::NodeDef node : graph.node())
    {
        nodes.insert({ node.name(), node });
        output_names.insert(node.name());
    }

    for (tensorflow::NodeDef node : graph.node())
    {
        for (std::string input_name : node.input())
        {
            if (output_names.find(input_name) != output_names.end())
            {
                output_names.erase(input_name);
            }
        }
    }

    tcc_assert(output_names.size() == 1,
               "tensorflow graph with no output or multiple outputs is not "
               "supported.");
    std::string output_name = *output_names.begin();

    /* recurive traverse the tensorflow graph and
     * parse each tensorflow node into hlir. */
    std::unordered_map<std::string, hlir::expr> parsed_nodes;

    recurse_graph(output_name, nodes, parsed_nodes, input_shapes);

    tcc_assert_has_key(parsed_nodes, output_name);
    return parsed_nodes.at(output_name);
}

hlir::expr
parse(std::string input_path,
      std::unordered_map<std::string, std::vector<long>>& input_shapes)
{
    tensorflow::GraphDef graph = load_graph(input_path);
    return parse_graph(graph, input_shapes);
}

} // namespace frontend
} // namespace tcc
