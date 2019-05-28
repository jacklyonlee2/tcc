#include "tcc/parser/parser.h"

#include <fstream>

#include "tcc/core/operator/operator_registry.h"
#include "protos/graph.pb.h"
#include "protos/node_def.pb.h"
#include "tcc/util/logging.h"

namespace tcc {
namespace parser {

void ParseFrozenGraph(std::string file_path) {
    std::fstream file(file_path, std::ios::in | std::ios::binary);

    tensorflow::GraphDef tf_graph;
    CHECK(tf_graph.ParseFromIstream(&file)) <<
        "Failed to parse frozen graph at'" << file_path << "'.";

    for (tensorflow::NodeDef tf_node : tf_graph.node()) {
        LOG(ERROR) << tf_node.op() << " - " << tf_node.name();
    }
    LOG(ERROR) << "Total nodes: " << tf_graph.node_size();

    core::op::OperatorRegistry::Instantiate("Test");
}

} // namespace parser
} // namespace tcc
