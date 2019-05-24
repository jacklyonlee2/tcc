#include "tcc/parser/parser.h"

#include <fstream>

#include "tcc/core/operator_registry.h"
#include "protos/graph.pb.h"
#include "protos/node_def.pb.h"
#include "tcc/util/logging.h"

using namespace ::tcc::core;

namespace tcc {
namespace parser {

void ParseFrozenGraph(std::string file_path) {
    std::fstream file(file_path, std::ios::in | std::ios::binary);

    tensorflow::GraphDef tf_graph;
    CHECK(tf_graph.ParseFromIstream(&file)) <<
        "Failed to parse frozen graph at'" << file_path << "'.";

    for (tensorflow::NodeDef tf_node : tf_graph.node()) {
        LOG(INFO) << tf_node.op() << " - " << tf_node.name();
    }
    LOG(INFO) << "Total nodes: " << tf_graph.node_size();

    OperatorRegistry::Instantiate("Test");
}

} // namespace parser
} // namespace tcc
