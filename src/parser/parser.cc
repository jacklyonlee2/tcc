#include "tcc/parser/parser.h"

#include <fstream>
#include <unordered_set>

#include "proto/graph.pb.h"
#include "tcc/core/context.h"

using namespace ::tcc::core;

namespace tcc {
namespace parser {

static void ParseFile(ParserContext& ctx, tensorflow::GraphDef& graph) {
    // Deserialize frozen graph into GraphDef.
    std::fstream file;
    file.open(ctx.input_path_, std::ios::in | std::ios::binary);
    CHECK(file) << "Failed to open file at '" << ctx.input_path_ << "'.";
    CHECK(graph.ParseFromIstream(&file)) <<
        "Failed to parse frozen graph at'" << ctx.input_path_ << "'.";
    file.close();
}

static void CheckNodes(ParserContext& ctx, tensorflow::GraphDef graph,
        std::unordered_set<std::string> ignore = {
        "Const",
        "Placeholder"}) {
    // Check all node types are registered.
    for (tensorflow::NodeDef node : graph.node()) {
        bool node_registered = ctx.OperatorExists(node.op());
        bool node_ignored = ignore.find(node.op()) != ignore.end();
        CHECK(node_registered || node_ignored) << "Unregistered operator '" << node.op() << "'.";
    }
}

static void ParseAttrs(Operator& op, tensorflow::NodeDef node,
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
                            op.SetAttr(attr_kv.first, Data(list.i().data(), list.i().size()));

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
                    op.SetAttr(attr_kv.first, Data(attr_kv.second.s()));
                    break;
                }
                case tensorflow::AttrValue::kF: { // float - 3
                    op.SetAttr(attr_kv.first, Data(attr_kv.second.f()));
                    break;
                }
                case tensorflow::AttrValue::VALUE_NOT_SET: { // notset - 0
                    LOG(FATAL) << "Unset tensorflow attr value.";
                }
                default: { // Unsupported
                    LOG(FATAL) << "Unsupported tensorflow attr '" << attr_kv.first <<
                        "' of type '" << attr_kv.second.value_case() << "'.";
                }
            }
        }
    }
}

static Datatype ParseDatatype(tensorflow::DataType type) {
    // Convert tensorflow::DataType to tcc::Datatype
    switch (type) {
        case tensorflow::DT_FLOAT: // 1
            return Datatype::kTensorFloat32;
        case tensorflow::DT_INT32: // 3
            return Datatype::kTensorInt32;
        default:
            LOG(FATAL) << "Unsupported datatype conversion tensorflow datatype '" << type << "'.";
    }
}

template <typename T>
static Data ParseDataHelper(tensorflow::TensorProto tensor, std::vector<int64_t> shape) {
    char* bytes = strdup(tensor.tensor_content().data());
    T* data = reinterpret_cast<T*>(bytes);
    return Data(data, shape);
}

static Data ParseData(tensorflow::TensorProto tensor) {
    // Convert tensorflow::TensorProto to tcc::Data
    CHECK(tensor.has_tensor_shape()) << "Tensor is missing shape.";
    CHECK(tensor.tensor_shape().dim_size() > 0) << "Tensor shape is empty.";

    // Parse tensor shape
    std::vector<int64_t> shape;
    for (tensorflow::TensorShapeProto_Dim dim : tensor.tensor_shape().dim()) {
        shape.push_back(dim.size());
    }

    switch (ParseDatatype(tensor.dtype())) {
        case Datatype::kTensorFloat32:
            return ParseDataHelper<float>(tensor, shape);
        case Datatype::kTensorInt32:
            return ParseDataHelper<int32_t>(tensor, shape);
        default:
            LOG(FATAL) << "Unsupported datatype for data conversion.";
    }
}

static void ParseNodes(ParserContext& ctx, tensorflow::GraphDef graph) {
    // Parse NodeDef into HLIR operations / variables.
    for (tensorflow::NodeDef node : graph.node()) {

        if (node.op() == "Placeholder") { // Placeholder op
            CHECK_KEY_IN_MAP("dtype", node.attr());
            CHECK(node.attr().at("dtype").value_case() == tensorflow::AttrValue::kType) <<
                "Attr 'dtype' must be of type tensorflow::AttrValue::kType.";

            std::string variable_name = node.name();
            Datatype datatype =
                ParseDatatype(node.attr().at("dtype").type());

            //hlir.AddVariable({variable_name, datatype});

        } else if (node.op() == "Const") { // Const op
            CHECK_KEY_IN_MAP("value", node.attr());
            CHECK(node.attr().at("value").value_case() == tensorflow::AttrValue::kTensor) <<
                "Attr 'value' must be of type tensorflow::AttrValue::kTensor.";

            std::string variable_name = node.name();
            Data data =
                ParseData(node.attr().at("value").tensor());

            //hlir.AddVariable({variable_name, data});

        } else { // Other ops
            std::string operation_name = node.name();
            Operator op = ctx.OperatorInstantiate(node.op());
            ParseAttrs(op, node);

            //hlir.AddOperation({operation_name, op});
        }
    }
}

void TensorFlowParser(ParserContext& ctx) {
    tensorflow::GraphDef graph;
    ParseFile(ctx, graph);
    CheckNodes(ctx, graph);
    ParseNodes(ctx, graph);
}

} // namespace parser
} // namespace tcc
