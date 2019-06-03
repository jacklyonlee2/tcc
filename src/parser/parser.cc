#include "tcc/parser/parser.h"

#include <fstream>
#include <unordered_set>

#include "tcc/core/common/operator_registry.h"
#include "tcc/core/common/operator.h"
#include "protos/graph.pb.h"
#include "protos/node_def.pb.h"
#include "tcc/util/logging.h"

namespace tcc {
namespace parser {

static void ParseFile(tensorflow::GraphDef& graph, std::string file_path) {
    // Deserialize frozen graph into GraphDef.
    std::fstream file;
    file.open(file_path, std::ios::in | std::ios::binary);
    CHECK(file) << "Failed to open file at '" << file_path << "'.";
    CHECK(graph.ParseFromIstream(&file)) <<
        "Failed to parse frozen graph at'" << file_path << "'.";
    file.close();
}

static void CheckNodes(tensorflow::GraphDef graph) {
    // Check all node types are registered.
    bool all_registered = true;
    std::unordered_set<std::string> ignore({"Const", "Placeholder"});

    for (tensorflow::NodeDef node : graph.node()) {
        bool node_registered = core::common::OperatorRegistry::Registered(node.op());
        bool node_ignored = ignore.find(node.op()) != ignore.end();

        if (!node_registered && !node_ignored) {
            LOG(ERROR) << "Unregistered operator '" << node.op() << "'.";
            ignore.insert(node.op());
            all_registered = false;
        }
    }

    CHECK(all_registered) << "Graph constains unregistered operator types.";
}

static void ParseAttrs(core::common::Operator& op, tensorflow::NodeDef node) {
}

static void ParseNodes(core::hlir::HLIR& hlir, tensorflow::GraphDef graph) {
    // Parse NodeDef into HLIR operations / variables.
    for (tensorflow::NodeDef node : graph.node()) {

        if (node.op() == "Placeholder") {

        } else if (node.op() == "Const") {

        } else {
            core::common::Operator op = core::common::OperatorRegistry::Instantiate(node.op());
            ParseAttrs(op, node);
        }
    }
}

static void ParseEdges(core::hlir::HLIR& hlir, tensorflow::GraphDef graph) {
    // Parse NodeDef edges into HLIR variables
    for (tensorflow::NodeDef node : graph.node()) {
    }
}

void ParseFrozenGraph(core::hlir::HLIR& hlir, std::string file_path) {
    tensorflow::GraphDef graph;
    ParseFile(graph, file_path);
    CheckNodes(graph);
    ParseNodes(hlir, graph);
    ParseEdges(hlir, graph);
}

} // namespace parser
} // namespace tcc
