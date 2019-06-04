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

static void CheckNodes(tensorflow::GraphDef graph,
        std::unordered_set<std::string> ignore = {
        "Const",
        "Placeholder"}) {
    // Check all node types are registered.
    for (tensorflow::NodeDef node : graph.node()) {
        bool node_registered = core::common::OperatorRegistry::Registered(node.op());
        bool node_ignored = ignore.find(node.op()) != ignore.end();
        CHECK(node_registered || node_ignored) << "Unregistered operator '" << node.op() << "'.";
    }
}

static void ParseAttrs(core::common::Operator& op, tensorflow::NodeDef node,
        std::unordered_set<std::string> ignore = {
        "T",
        "Tshape",
        "out_type",
        "use_cudnn_on_gpu",
        "is_training"}) {
    // Parse AttrValue and add to op
    for (std::pair<std::string, tensorflow::AttrValue> attr_kv : node.attr()) {
        if (ignore.find(attr_kv.first) == ignore.end()) {

            switch(attr_kv.second.value_case()) {
                case tensorflow::AttrValue::kList: { // list - 1
                        tensorflow::AttrValue_ListValue list = attr_kv.second.list();
                        if (list.s_size() > 0) { // list(string) - 2
                            LOG(FATAL) << "Unsupported tensorflow attr list type 'list(string)'.";

                        } else if (list.i_size() > 0) { // list(int) - 3
                            op.SetAttr(attr_kv.first, core::common::Data(
                                        std::vector<int64_t>(list.i().begin(), list.i().end())));

                        } else if (list.f_size() > 0) { // list(float) - 4
                            LOG(FATAL) << "Unsupported tensorflow attr list type 'list(float)'.";

                        } else if (list.b_size() > 0) { // list(bool) - 5
                            LOG(FATAL) << "Unsupported tensorflow attr list type 'list(bool)'.";

                        } else if (list.type_size() > 0) { // list(type) - 6
                            LOG(FATAL) << "Unsupported tensorflow attr list type 'list(type)'.";

                        } else if (list.shape_size() > 0) { // list(shape) - 7
                            LOG(FATAL) << "Unsupported tensorflow attr list type 'list(shape)'.";

                        } else if (list.tensor_size() > 0) { // list(tensor) - 8
                            LOG(FATAL) << "Unsupported tensorflow attr list type 'list(tensor)'.";

                        } else if (list.func_size() > 0) { // list(attr) - 9
                            LOG(FATAL) << "Unsupported tensorflow attr list type 'list(attr)'.";

                        } else LOG(FATAL) << "Unknown tensorflow attr list type.";
                        break;
                }
                case tensorflow::AttrValue::kS: { // string - 2
                    op.SetAttr(attr_kv.first, core::common::Data(attr_kv.second.s()));
                    break;
                }
                case tensorflow::AttrValue::kF: { // float - 3
                    op.SetAttr(attr_kv.first, core::common::Data(attr_kv.second.f()));
                    break;
                }
                case tensorflow::AttrValue::VALUE_NOT_SET: { // notset - 0
                    LOG(FATAL) << "Unset tensorflow attr value.";
                }
                default: { // Unsupported
                    LOG(FATAL) << "Unknown tensorflow attr '" << attr_kv.first <<
                        "' of type '" << attr_kv.second.value_case() << "'.";
                }
            }
        }
    }
}

static void ParseNodes(core::hlir::HLIR& hlir, tensorflow::GraphDef graph) {
    // Parse NodeDef into HLIR operations / variables.
    for (tensorflow::NodeDef node : graph.node()) {

        if (node.op() == "Placeholder") {

        } else if (node.op() == "Const") {

        } else {
            core::common::Operator op =
                core::common::OperatorRegistry::Instantiate(node.op());
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
