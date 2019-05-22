#include "tcc/parser/parser.h"

#include <fstream>

#include "tcc/core/operator_registry.h"
#include "protos/graph.pb.h"
#include "protos/node_def.pb.h"
#include "glog/logging.h"

using namespace std;

void tcc::parser::ParseFrozenGraph(string file_path) {
    fstream file(file_path, ios::in | ios::binary);
    CHECK(file) << "file at '" << file_path << "' not found.";

    tensorflow::GraphDef tf_graph;
    CHECK(tf_graph.ParseFromIstream(&file)) <<
        "Failed to parser frozen graph at'" << file_path << "'.";

    for (tensorflow::NodeDef tf_node : tf_graph.node()) {
        LOG(INFO) << tf_node.op() << " - " << tf_node.name();
    }
    LOG(INFO) << "Total nodes: " << tf_graph.node_size();

    if (tcc::core::OperatorRegistry::GetTest("Test")) {
        LOG(INFO) << "registered.";
    }
}
