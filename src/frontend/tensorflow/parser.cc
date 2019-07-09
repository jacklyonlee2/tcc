#include "tcc/frontend/frontend.h"

#include <fstream>
#include <functional>

#include "tcc/core/common/data.h"
#include "proto/graph.pb.h"

using namespace ::tcc::core;
using namespace ::google::protobuf;

namespace tcc {
namespace frontend {

static tensorflow::GraphDef parse_file(std::string input_path) {
    std::fstream file;
    file.open(input_path, std::ios::in | std::ios::binary);
    CHECK(file) <<
        "Failed to open file at " << input_path << ".";

    tensorflow::GraphDef graph;
    CHECK(graph.ParseFromIstream(&file)) <<
        "Failed to parse frozen graph at " << input_path << ".";
    file.close();

    return graph;
}

static DataType parse_data_type(tensorflow::DataType type) {
    switch (type) {
        case tensorflow::DT_FLOAT: // 1
            return DataType::FLOAT;
        case tensorflow::DT_INT32: // 3
            return DataType::INT;
        default:
            LOG(FATAL) << "Unsupported DataType datatype " << type << ".";
    }
}

static DataDesc parse_data_desc(
        Map<std::string, tensorflow::AttrValue> attrs,
        std::vector<long> shape) {
    CHECK_KEY_IN_MAP("dtype", attrs) <<
        "Attribute 'dtype' not found.";
    tensorflow::AttrValue dtype = attrs.at("dtype");

    CHECK(dtype.value_case() == tensorflow::AttrValue::kType) <<
        "Attribute 'dtype' must be of type tensorflow::AttrValue::kType.";
    DataType type = parse_data_type(dtype.type());

    return DataDesc(type, shape);
}

template<typename T>
static Data parse_binary(const char *content_str, std::vector<long> shape) {
    const T *content_ptr = reinterpret_cast<T*>(strdup(content_str));
    std::vector<T> content(
            content_ptr,
            content_ptr + static_cast<size_t>(
                std::accumulate(
                    shape.begin(), shape.end(),
                    1l, std::multiplies<long>())));
    return Data(content, shape);
}

static Data parse_data(
        Map<std::string, tensorflow::AttrValue> attrs) {
    CHECK_KEY_IN_MAP("value", attrs) <<
        "Attribute 'value' not found.";
    tensorflow::AttrValue value = attrs.at("value");

    CHECK(value.value_case() == tensorflow::AttrValue::kTensor) <<
        "Attribute 'value' must be of type tensorflow::AttrValue::kTensor.";
    tensorflow::TensorProto tensor = value.tensor();

    CHECK(tensor.has_tensor_shape()) << "Tensor is missing shape.";
    CHECK(tensor.tensor_shape().dim_size() > 0) << "Tensor shape is empty.";

    /* Parse tensor shape. */
    std::vector<long> shape;
    for (tensorflow::TensorShapeProto_Dim dim : tensor.tensor_shape().dim()) {
        shape.push_back(dim.size());
    }

    switch (parse_data_type(tensor.dtype())) {
        case DataType::FLOAT:
            return parse_binary<float>(tensor.tensor_content().data(), shape);
        case DataType::INT:
            return parse_binary<int>(tensor.tensor_content().data(), shape);
        default:
            LOG(FATAL) << "Unsupported DataType.";
    }
}

/* Macros expanding into attribute parsing functions
 * for each TensorFlow datatype. */

#define ATTR_CHECK(attr_name, attrs, attr_val, tf_enum) \
    CHECK_KEY_IN_MAP(attr_name, attrs) << \
        "Attribute '" << attr_name << "' not found."; \
    attr_val = attrs.at(attr_name); \
    CHECK(attr_val.value_case() == tensorflow::AttrValue::tf_enum) << \
        "Attribute '" << attr_name << "' must be of type " << \
        "tensorflow::AttrValue::" << #tf_enum << "."

#define SCALAR_ATTR_PARSER(funcname, tf_enum, accessor, type) \
    static type funcname( \
            Map<std::string, tensorflow::AttrValue> attrs, \
            std::string attr_name) { \
        tensorflow::AttrValue attr_val; \
        ATTR_CHECK(attr_name, attrs, attr_val, tf_enum); \
        return attr_val.accessor(); \
    }

#define VECTOR_ATTR_PARSER(funcname, accessor, type) \
    static std::vector<type> funcname( \
            Map<std::string, tensorflow::AttrValue> attrs, \
            std::string attr_name) { \
        tensorflow::AttrValue attr_val; \
        ATTR_CHECK(attr_name, attrs, attr_val, kList); \
        tensorflow::AttrValue_ListValue list = attr_val.list(); \
        CHECK(list.accessor##_size() > 0) << \
            "List attribute datatype list(" << #type << ") is not found."; \
        const type *content = list.accessor().data(); \
        return std::vector<type>(content, content + list.accessor##_size()); \
    }

SCALAR_ATTR_PARSER(parse_scalar_string, kS, s, std::string)
SCALAR_ATTR_PARSER(parse_scalar_float, kF, f, float)
VECTOR_ATTR_PARSER(parse_vector_long, i, long)

#undef ATTR_CHECK
#undef SCALAR_ATTR_PARSER
#undef VECTOR_ATTR_PARSER

static std::pair<Op, std::vector<Op>> parse_node(
        tensorflow::NodeDef node,
        std::vector<Op> input_ops,
        std::unordered_map<std::string, std::vector<long>> input_shapes) {
    /* Parse TensorFlow node according to op type. */
    std::pair<Op, std::vector<Op>> ret;
    std::string op_type = node.op();
    auto attrs = node.attr();

    if (op_type == "Placeholder") {
        CHECK(input_ops.size() == 0);
        CHECK_KEY_IN_MAP(node.name(), input_shapes) <<
            "Missing input shape for TensorFlow node '" << node.name() << "'.";

        DataDesc data_desc =
            parse_data_desc(attrs, input_shapes.at(node.name()));
        auto op = op::downcast<op::Placeholder>(
                op::Placeholder::make(data_desc));

        ret = {op, {}};

    } else if (op_type == "Const") {
        CHECK(input_ops.size() == 0);

        Data data = parse_data(attrs);
        auto op = op::downcast<op::Constant>(
                op::Constant::make(data));

        ret = {op, {}};

    } else if (op_type == "Add") {
        CHECK(input_ops.size() == 2);

        Op x = input_ops[0];
        Op y = input_ops[1];
        auto op = op::downcast<op::Add>(
                op::Add::make(x, y));

        ret = {op, {op->z}};

    } else if (op_type == "AvgPool") {
        CHECK(input_ops.size() == 1);

        std::string data_format =
            parse_scalar_string(attrs, "data_format");
        std::string padding =
            parse_scalar_string(attrs, "padding");
        std::vector<long> ksize =
            parse_vector_long(attrs, "ksize");
        std::vector<long> strides =
            parse_vector_long(attrs, "strides");
        Op value = input_ops[0];
        auto op = op::downcast<op::AvgPool>(
                op::AvgPool::make(
                    data_format,
                    padding,
                    ksize,
                    strides,
                    value));

        ret = {op, {op->output}};

    } else if (op_type == "BiasAdd") {
        CHECK(input_ops.size() == 2);

        std::string data_format =
            parse_scalar_string(attrs, "data_format");
        Op input = input_ops[0];
        Op bias = input_ops[1];
        auto op = op::downcast<op::BiasAdd>(
                op::BiasAdd::make(
                    data_format,
                    input,
                    bias));

        ret = {op, {op->output}};

    } else if (op_type == "Conv2D") {
        CHECK(input_ops.size() == 2);

        std::string data_format =
            parse_scalar_string(attrs, "data_format");
        std::string padding =
            parse_scalar_string(attrs, "padding");
        std::vector<long> strides =
            parse_vector_long(attrs, "strides");
        std::vector<long> dilations =
            parse_vector_long(attrs, "dilations");
        Op input = input_ops[0];
        Op filter = input_ops[1];
        auto op = op::downcast<op::Conv2D>(
                op::Conv2D::make(
                    data_format,
                    padding,
                    strides,
                    dilations,
                    input,
                    filter));

        ret = {op, {op->output}};

    } else if (op_type == "DepthwiseConv2dNative") {
        CHECK(input_ops.size() == 2);

        std::string data_format =
            parse_scalar_string(attrs, "data_format");
        std::string padding =
            parse_scalar_string(attrs, "padding");
        std::vector<long> strides =
            parse_vector_long(attrs, "strides");
        std::vector<long> dilations =
            parse_vector_long(attrs, "dilations");
        Op input = input_ops[0];
        Op filter = input_ops[1];
        auto op = op::downcast<op::DepthwiseConv2dNative>(
                op::DepthwiseConv2dNative::make(
                    data_format,
                    padding,
                    strides,
                    dilations,
                    input,
                    filter));

        ret = {op, {op->output}};

    } else if (op_type == "FusedBatchNorm") {
        CHECK(input_ops.size() == 5);

        float epsilon =
            parse_scalar_float(attrs, "epsilon");
        std::string data_format =
            parse_scalar_string(attrs, "data_format");
        Op x = input_ops[0];
        Op scale = input_ops[1];
        Op offset = input_ops[2];
        Op mean = input_ops[3];
        Op variance = input_ops[4];
        auto op = op::downcast<op::FusedBatchNorm>(
                op::FusedBatchNorm::make(
                    epsilon,
                    data_format,
                    x,
                    scale,
                    offset,
                    mean,
                    variance));

        ret = {op, {op->y}};

    } else if (op_type == "Relu6") {
        CHECK(input_ops.size() == 1);

        Op features = input_ops[0];
        auto op = op::downcast<op::Relu6>(
                op::Relu6::make(features));

        ret = {op, {op->activations}};

    } else if (op_type == "Reshape") {
        CHECK(input_ops.size() == 2);

        Op tensor = input_ops[0];
        Op shape = input_ops[1];
        auto op = op::downcast<op::Reshape>(
                op::Reshape::make(
                    tensor,
                    shape));

        ret = {op, {op->output}};

    } else if (op_type == "Shape") {
        CHECK(input_ops.size() == 1);

        Op input = input_ops[0];
        auto op = op::downcast<op::Shape>(
                op::Shape::make(input));

        ret = {op, {op->output}};

    } else if (op_type == "Softmax") {
        CHECK(input_ops.size() == 1);

        Op logits = input_ops[0];
        auto op = op::downcast<op::Softmax>(
                op::Softmax::make(logits));

        ret = {op, {op->output}};

    } else if (op_type == "Squeeze") {
        CHECK(input_ops.size() == 1);

        std::vector<long> squeeze_dims =
            parse_vector_long(attrs, "squeeze_dims");
        Op input = input_ops[0];
        auto op = op::downcast<op::Squeeze>(
                op::Squeeze::make(
                    squeeze_dims,
                    input));

        ret = {op, {op->output}};

    } else  {
        LOG(FATAL) << "Unsupported op type '" << op_type << "'.";
    }

    return ret;
}

static HLIR parse_graph(
        tensorflow::GraphDef graph,
        std::unordered_map<std::string, std::vector<long>> input_shapes) {
    /* Map all nodes in TensorFlow graph.
     * Find output nodes of TensorFlow graph. */
    std::unordered_map<std::string, tensorflow::NodeDef> node_map;
    std::unordered_set<std::string> output_nodes;
    for (tensorflow::NodeDef node : graph.node()) {
        node_map.insert({node.name(), node});
        output_nodes.insert(node.name());
    }

    for (tensorflow::NodeDef node : graph.node()) {
        for (std::string input_name : node.input()) {
            CHECK_KEY_IN_MAP(input_name, node_map) <<
                "Tensorflow node '" << input_name << "' is not found in node_map.";
            if (output_nodes.find(input_name) != output_nodes.end()) {
                output_nodes.erase(input_name);
            }
        }
    }

    /* Map of all HLIR Ops to keep shared ptrs alive
     * and map terminal node to be used as inputs. */
    std::unordered_map<std::string, Op> hlir_op_map;
    /* Map of all HLIR Ops' outputs (Intermediate).
     * Used to feed to other HLIR Op as inputs. */
    std::unordered_map<std::string, std::vector<Op>> hlir_outputs_map;
    /* Lambda function recursively traverse Tensorflow graph backwards
     * and build HLIR on return. */
    std::function<void(tensorflow::NodeDef&)> recurse_node;
    recurse_node = [&recurse_node, &input_shapes, &node_map,
        &hlir_op_map, &hlir_outputs_map](tensorflow::NodeDef& node) {
        /* Collect input HLIR Ops */
        std::vector<Op> input_ops;
        for (std::string input_name : node.input()) {
            CHECK_KEY_IN_MAP(input_name, node_map) <<
                "Tensorflow node '" << input_name << "' is not found in node_map.";
            tensorflow::NodeDef input_node = node_map.at(input_name);

            if (input_node.op() == "Placeholder" || input_node.op() == "Const") {
                /* Recurse further to reach Placeholder or Const input. */
                if (hlir_op_map.find(input_name) == hlir_op_map.end()) {
                    recurse_node(input_node);
                }

                /* Base case: input Placeholder or Const
                 * has already been traversed. */
                CHECK_KEY_IN_MAP(input_name, hlir_op_map) <<
                     "'" << input_name << "' is not found in hlir_op_map.";
                input_ops.push_back(hlir_op_map.at(input_name));
            } else {
                /* Recurse further to reach input op.
                 * TODO: take care of multiple output case. */
                if (hlir_outputs_map.find(input_name) == hlir_outputs_map.end()) {
                    recurse_node(input_node);
                }

                /* Base case: input op has already been traversed.
                 * TODO: take care of multiple output case. */
                CHECK_KEY_IN_MAP(input_name, hlir_outputs_map) <<
                    "'" << input_name << "' is not found in hlir_outputs_map.";
                CHECK(!hlir_outputs_map.at(input_name).empty()) <<
                    "Empty hlir_outputs_map entry at '" << input_name << "'.";
                input_ops.push_back(hlir_outputs_map.at(input_name)[0]);
            }
        }

        /* Create and store current HLIR Op */
        std::pair<Op, std::vector<Op>> ret =
            parse_node(node, input_ops, input_shapes);
        hlir_op_map.insert({node.name(), ret.first});
        hlir_outputs_map.insert({node.name(), ret.second});
    };

    /* Invoke recurse_node on all Tensorflow output nodes. */
    for (std::string output_node : output_nodes) {
        CHECK_KEY_IN_MAP(output_node, node_map) <<
            "Tensorflow node '" << output_node << "' not found in node_map.";
        recurse_node(node_map.at(output_node));
    }

    /* Collect HLIR ops. */
    std::unordered_set<Op> hlir_ops;
    for (std::pair<std::string, Op> op_kv : hlir_op_map) {
        hlir_ops.insert(op_kv.second);
    }
    for (std::pair<std::string, std::vector<Op>> op_outputs_kv : hlir_outputs_map) {
        for (Op op : op_outputs_kv.second) {
            hlir_ops.insert(op);
        }
    }

    return HLIR(hlir_ops);
}

HLIR parse_tensorflow(
        std::string input_path,
        std::unordered_map<std::string, std::vector<long>> input_shapes) {
    /* Deserialize TensorFlow frozen graph. */
    tensorflow::GraphDef graph = parse_file(input_path);
    HLIR hlir = parse_graph(graph, input_shapes);
    return hlir;
}

} // namespace frontend
} // namespace tcc
