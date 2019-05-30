#include "tcc/parser/parser.h"

#include <fstream>

#include "tcc/core/operator/operator_registry.h"
#include "tcc/core/operator/operator.h"
#include "protos/graph.pb.h"
#include "protos/node_def.pb.h"
#include "tcc/util/logging.h"

namespace tcc {
namespace parser {

static void ParseFile(tensorflow::GraphDef& graph, std::string file_path) {
    std::fstream file;
    file.open(file_path, std::ios::in | std::ios::binary);
    CHECK(file) << "Failed to open file at '" << file_path << "'.";

    CHECK(graph.ParseFromIstream(&file)) <<
        "Failed to parse frozen graph at'" << file_path << "'.";
    file.close();
}

static void ParseAttrs(core::op::Operator& op, tensorflow::NodeDef node) {
}

static void ParseEdges(core::hlir::HLIR& hlir, tensorflow::NodeDef node) {
}

static void ParseNode(core::hlir::HLIR& hlir, tensorflow::NodeDef node) {
    LOG(ERROR) << node.op() << " - " << node.name();
}

void ParseFrozenGraph(core::hlir::HLIR& hlir, std::string file_path) {
    // Deserialize frozen graph into GraphDef
    tensorflow::GraphDef graph;
    ParseFile(graph, file_path);

    // Parse all nodes and add them to HLIR
    for (tensorflow::NodeDef node : graph.node()) {
        ParseNode(hlir, node);
    }
}

} // namespace parser
} // namespace tcc
