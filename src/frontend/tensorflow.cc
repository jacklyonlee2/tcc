#include "tcc/frontend/frontend.h"

#include <fstream>
#include <unordered_set>
#include <functional>

#include "tcc/core/operator.h"
#include "tensorflow_proto/graph.pb.h"

using namespace ::tcc::core;

namespace tcc {
namespace frontend {

static tensorflow::GraphDef ParseFile(std::string input_path) {
    std::fstream file;
    file.open(input_path, std::ios::in | std::ios::binary);
    CHECK(file) <<
        "Failed to open file at" QUOTE_MSG(input_path) ".";

    tensorflow::GraphDef graph;
    CHECK(graph.ParseFromIstream(&file)) <<
        "Failed to parse frozen graph at" QUOTE_MSG(input_path) ".";
    file.close();

    return graph;
}

static bool CheckNodes(
        tensorflow::GraphDef graph,
        std::unordered_set<std::string> ignore = {
        "Const",
        "Placeholder"}) {
    // Check all node types are registered operators.
    bool all_registered = true;
    for (tensorflow::NodeDef node : graph.node()) {
        if (ignore.find(node.op()) == ignore.end() /* node type is not ignored */ &&
                !Operator::Exists(node.op()) /* node type is not registered */ ) {
            LOG(ERROR) << "Unregistered operator" QUOTE_MSG(node.op()) ".";
            all_registered = false;
        }
    }

    return all_registered;
}

static std::unordered_map<std::string, Data> ParseAttrs(
        tensorflow::NodeDef node,
        std::unordered_set<std::string> ignore = {
        "T",
        "Tshape",
        "out_type",
        "use_cudnn_on_gpu",
        "is_training"}) {
    // Parse Attr to map of std::string and tcc::core::Data
    std::unordered_map<std::string, Data> attr_map;
    for (std::pair<std::string, tensorflow::AttrValue> attr_kv : node.attr()) {
        if (ignore.find(attr_kv.first) == ignore.end()) {

            switch(attr_kv.second.value_case()) {
                case tensorflow::AttrValue::kList: { // list - 1
                        tensorflow::AttrValue_ListValue list = attr_kv.second.list();
                        if (list.s_size() > 0) { // list(string) - 2
                            LOG(FATAL) << "Unsupported tensorflow attr list type 'list(string)'.";

                        } else if (list.i_size() > 0) { // list(int) - 3
                            attr_map.insert({attr_kv.first, Data::kTensorI64(list.i().data(), list.i().size())});

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
                    attr_map.insert({attr_kv.first, Data::kScalarSTR(attr_kv.second.s())});
                    break;
                }
                case tensorflow::AttrValue::kF: { // float - 3
                    attr_map.insert({attr_kv.first, Data::kScalarFP32(attr_kv.second.f())});
                    break;
                }
                case tensorflow::AttrValue::VALUE_NOT_SET: { // notset - 0
                    LOG(FATAL) << "Unset tensorflow attr value.";
                }
                default: { // Unsupported
                    LOG(FATAL) << "Unsupported tensorflow attr" QUOTE_MSG(attr_kv.first)
                        "of type" QUOTE_MSG(attr_kv.second.value_case()) ".";
                }
            }
        }
    }

    return attr_map;
}

static Data ParsePlaceholder(tensorflow::DataType type, std::vector<long> shape) {
    // Convert tensorflow::DataType to tcc::core::Datatype
    switch (type) {
        case tensorflow::DT_FLOAT: // 1
            return Data::kTensorFP32(shape);
        case tensorflow::DT_INT32: // 3
            return Data::kTensorI32(shape);
        default:
            LOG(FATAL) << "Unsupported datatype conversion tensorflow datatype"
                QUOTE_MSG(type) ".";
    }
}

static Data ParseConst(tensorflow::TensorProto tensor) {
    // Convert tensorflow::TensorProto to tcc::core::Data
    CHECK(tensor.has_tensor_shape()) << "Tensor is missing shape.";
    CHECK(tensor.tensor_shape().dim_size() > 0) << "Tensor shape is empty.";

    // Parse tensor shape
    std::vector<int64_t> shape;
    for (tensorflow::TensorShapeProto_Dim dim : tensor.tensor_shape().dim()) {
        shape.push_back(dim.size());
    }

    switch (tensor.dtype()) {
        case tensorflow::DT_FLOAT:
            return Data::kTensorFP32(tensor.tensor_content().data(), shape);
        case tensorflow::DT_INT32:
            return Data::kTensorI32(tensor.tensor_content().data(), shape);
        default:
            LOG(FATAL) << "Unsupported datatype for data conversion.";
    }
}

static HLIR ParseNodes(tensorflow::GraphDef graph,
        std::unordered_map<std::string, std::vector<long>> input_shapes) {
    // Parse tensorflow::GraphDef into tcc::core::HLIR
    // Put all tensorflow::NodeDef into map
    // Find tensorflow::NodeDef that are outputs of the graph
    std::unordered_map<std::string, tensorflow::NodeDef> node_map;
    std::unordered_set<std::string> output_nodes;
    for (tensorflow::NodeDef node : graph.node()) {
        node_map.insert({node.name(), node});
        output_nodes.insert(node.name());
    }

    for (tensorflow::NodeDef node : graph.node()) {
        for (std::string input_name : node.input()) {
            CHECK_KEY_IN_MAP(input_name, node_map) <<
                "Tensorflow node input" QUOTE_MSG(input_name) "is not found in node_map.";
            if (output_nodes.find(input_name) != output_nodes.end()) {
                output_nodes.erase(input_name);
            }
        }
    }

    std::unordered_map<std::string, HLIR::VariablePtr> hlir_variables;
    std::unordered_map<std::string, HLIR::OperationPtr> hlir_operations;
    std::function<void(tensorflow::NodeDef&)> RecurseNodes;
    RecurseNodes = [&RecurseNodes, &input_shapes, &node_map, &hlir_variables, &hlir_operations](
            tensorflow::NodeDef& node) {
        // Recursively traverse tensorflow::GraphDef backwards
        // Construct HLIR::Operation and HLIR::Variable on return
        // and store the output HLIR::Variables in output_map

        // Construct HLIR::Operation
        HLIR::OperationPtr operation = std::make_shared<HLIR::Operation>(
                node.name(), node.op(), ParseAttrs(node));

        // Collect input HLIR::Variables
        std::vector<HLIR::VariablePtr> input_variables;
        for (std::string input_name : node.input()) {
            CHECK_KEY_IN_MAP(input_name, node_map) <<
                "Tensorflow node input" QUOTE_MSG(input_name) "is not found in node_map.";
            tensorflow::NodeDef input_node = node_map.at(input_name);

            if (hlir_variables.find(input_name) != hlir_variables.end()) {
                // Base case 1: variable has already been traversed
                // TODO: take care of multiple output case
                input_variables.push_back(hlir_variables.at(input_name));

            } else if (input_node.op() == "Const") {
                // Base case 2: reached "Const" node
                CHECK_KEY_IN_MAP("value", input_node.attr());
                tensorflow::AttrValue value = input_node.attr().at("value");

                CHECK(value.value_case() == tensorflow::AttrValue::kTensor) <<
                    "Attr 'value' must be of type tensorflow::AttrValue::kTensor.";
                HLIR::VariablePtr input_variable = std::make_shared<HLIR::Variable>(
                        input_name, ParseConst(value.tensor()));

                input_variables.push_back(input_variable);
                hlir_variables.insert({input_name, input_variable});

            } else if (input_node.op() == "Placeholder") {
                // Base case 3: reached "Placeholder" node
                CHECK_KEY_IN_MAP("dtype", input_node.attr());
                tensorflow::AttrValue dtype = input_node.attr().at("dtype");

                CHECK(dtype.value_case() == tensorflow::AttrValue::kType) <<
                    "Attr 'dtype' must be of type tensorflow::AttrValue::kType.";
                CHECK_KEY_IN_MAP(input_name, input_shapes) <<
                    KEY_NOT_FOUND_MSG(input_name, input_shapes);
                HLIR::VariablePtr input_variable = std::make_shared<HLIR::Variable>(
                        input_name, ParsePlaceholder(dtype.type(), input_shapes.at(input_name)));

                input_variables.push_back(input_variable);
                hlir_variables.insert({input_name, input_variable});

            } else {
                // TODO: take care of multiple output case
                if (hlir_operations.find(input_name) == hlir_operations.end()) {
                    RecurseNodes(input_node);
                }

                // Base case 4: input node has already been traversed
                input_variables.push_back(
                        hlir_operations.at(input_name)->GetOutputVariable(0));
            }
        }

        // Produce and store output HLIR::Variables
        std::vector<HLIR::VariablePtr> output_variables =
            HLIR::Connect(operation, input_variables);
        hlir_operations.insert({node.name(), operation});
        for (HLIR::VariablePtr output_variable : output_variables) {
            hlir_variables.insert({output_variable->instance_name_, output_variable});
        }
    };

    // Recurse on each output node and store the terminal output HLIR::Variables
    for (std::string output_node : output_nodes) {
        CHECK_KEY_IN_MAP(output_node, node_map) <<
            "Tensorflow node" QUOTE_MSG(output_node) "is not found in node_map.";
        RecurseNodes(node_map.at(output_node));
    }

    return HLIR(hlir_variables, hlir_operations);
}

HLIR FromTensorFlow(std::string input_path,
        std::unordered_map<std::string, std::vector<long>> input_shapes) {
    tensorflow::GraphDef graph = ParseFile(input_path);
    CHECK(CheckNodes(graph)) <<
        "TensorFlow frozen graph at" QUOTE_MSG(input_path) "is not supported.";

    return ParseNodes(graph, input_shapes);
}

} // namespace frontend
} // namespace tcc
