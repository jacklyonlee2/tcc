#include "tcc/frontend/parser.h"
#include "proto/graph.pb.h"
#include "tcc/common/logging.h"
#include "tcc/frontend/op.h"
#include <fstream>

namespace tcc {

static tensorflow::GraphDef load_graph(const std::string input_path)
{
    std::fstream file;
    file.open(input_path, std::ios::in | std::ios::binary);
    tcc_assert(file,
               "failed to open tensorflow frozen graph at " + input_path + ".");

    tensorflow::GraphDef graph;
    tcc_assert(graph.ParseFromIstream(&file),
               "failed to parse tensorflow frozen graph.");
    file.close();

    return graph;
}

static datatype parse_dtype(tensorflow::DataType dtype)
{
    switch (dtype)
    {
        case tensorflow::DT_FLOAT:
            return datatype::FP32;
        case tensorflow::DT_INT32:
            return datatype::INT32;
        default:
            tcc_error("unsupported tensorflow data type " +
                      std::to_string(dtype) + ".");
    }
}

static datatype parse_tensor_dtype(
    google::protobuf::Map<std::string, tensorflow::AttrValue> attrs)
{
    tcc_assert_has_key(attrs, "value");
    tensorflow::AttrValue value = attrs.at("value");

    tcc_assert(value.value_case() == tensorflow::AttrValue::kTensor,
               "tensorflow attribute value must be of type kTensor.");
    return parse_dtype(value.tensor().dtype());
}

static const std::string parse_tensor_data(
    google::protobuf::Map<std::string, tensorflow::AttrValue> attrs)
{
    tcc_assert_has_key(attrs, "value");
    tensorflow::AttrValue value = attrs.at("value");

    tcc_assert(value.value_case() == tensorflow::AttrValue::kTensor,
               "tensorflow attribute value must be of type kTensor.");
    return value.tensor().tensor_content();
}

static dimensions parse_tensor_shape(
    google::protobuf::Map<std::string, tensorflow::AttrValue> attrs)
{
    tcc_assert_has_key(attrs, "value");
    tensorflow::AttrValue value = attrs.at("value");

    tcc_assert(value.value_case() == tensorflow::AttrValue::kTensor,
               "tensorflow attribute value must be of type kTensor.");
    tensorflow::TensorProto tensor = value.tensor();

    tcc_assert(tensor.has_tensor_shape(),
               "tensorflow tensor is missing shape.");
    tcc_assert(tensor.tensor_shape().dim_size() > 0,
               "tensorflow tensor can not have empty shape.");

    dimensions shape;
    for (tensorflow::TensorShapeProto_Dim dim : tensor.tensor_shape().dim())
    {
        tcc_assert(
            dim.size() > 0,
            "tensorflow tensor shape can not have dimension of size zero.");
        shape.push_back(dim.size());
    }

    return shape;
}

static datatype parse_attr_dtype(
    google::protobuf::Map<std::string, tensorflow::AttrValue> attrs)
{
    tcc_assert_has_key(attrs, "dtype");
    tensorflow::AttrValue dtype = attrs.at("dtype");

    tcc_assert(dtype.value_case() == tensorflow::AttrValue::kType,
               "tensorflow attribute dtype must be of type kType.");

    return parse_dtype(dtype.type());
}

static std::string parse_attr_string(
    google::protobuf::Map<std::string, tensorflow::AttrValue> attrs,
    std::string attr_name)
{
    tcc_assert_has_key(attrs, attr_name);
    tensorflow::AttrValue attr_val = attrs.at(attr_name);

    tcc_assert(attr_val.value_case() == tensorflow::AttrValue::kS,
               "tensorflow attribute " + attr_name + " must be of type kS.");

    return attr_val.s();
}

static float parse_attr_float(
    google::protobuf::Map<std::string, tensorflow::AttrValue> attrs,
    std::string attr_name)
{
    tcc_assert_has_key(attrs, attr_name);
    tensorflow::AttrValue attr_val = attrs.at(attr_name);

    tcc_assert(attr_val.value_case() == tensorflow::AttrValue::kF,
               "tensorflow attribute " + attr_name + " must be of type kF.");

    return attr_val.f();
}

static dimensions parse_attr_int_vec(
    google::protobuf::Map<std::string, tensorflow::AttrValue> attrs,
    std::string attr_name)
{
    tcc_assert_has_key(attrs, attr_name);
    tensorflow::AttrValue attr_val = attrs.at(attr_name);

    tcc_assert(attr_val.value_case() == tensorflow::AttrValue::kList,
               "tensorflow attribute " + attr_name + " must be of type kList.");
    tensorflow::AttrValue_ListValue attr_val_list = attr_val.list();

    tcc_assert(attr_val_list.i_size() > 0,
               "tensorflow integer list attribute " + attr_name +
                   " can not be empty.");
    const int64_t* val = attr_val_list.i().data();

    return dimensions(val, val + attr_val_list.i_size());
}

static void parse_node(
    tensorflow::NodeDef& node,
    std::unordered_map<std::string, expr>& parsed_nodes,
    std::unordered_map<std::string, dimensions>& input_shapes)
{
    expr output;

    if (node.op() == "Placeholder")
    {
        tcc_assert_size_eq(node.input(), 0);

        datatype dtype = parse_attr_dtype(node.attr());
        tcc_assert_has_key(input_shapes, node.name());
        dimensions shape = input_shapes.at(node.name());

        output = build_placeholder(dtype, shape);
    }
    else if (node.op() == "Const")
    {
        tcc_assert_size_eq(node.input(), 0);

        const std::string data = parse_tensor_data(node.attr());
        datatype dtype = parse_tensor_dtype(node.attr());
        dimensions shape = parse_tensor_shape(node.attr());

        output = build_const(data, dtype, shape);
    }
    else if (node.op() == "Add")
    {
        tcc_assert_size_eq(node.input(), 2);

        expr x = parsed_nodes.at(node.input()[0]);
        expr y = parsed_nodes.at(node.input()[1]);

        output = build_add(x, y);
    }
    else if (node.op() == "AvgPool")
    {
        tcc_assert_size_eq(node.input(), 1);

        std::string data_format = parse_attr_string(node.attr(), "data_format");
        std::string padding = parse_attr_string(node.attr(), "padding");
        dimensions ksize = parse_attr_int_vec(node.attr(), "ksize");
        dimensions strides = parse_attr_int_vec(node.attr(), "strides");
        expr value = parsed_nodes.at(node.input()[0]);

        output = build_avgpool(data_format, padding, ksize, strides, value);
    }
    else if (node.op() == "BiasAdd")
    {
        tcc_assert_size_eq(node.input(), 2);

        std::string data_format = parse_attr_string(node.attr(), "data_format");
        expr input = parsed_nodes.at(node.input()[0]);
        expr bias = parsed_nodes.at(node.input()[1]);

        output = build_biasadd(data_format, input, bias);
    }
    else if (node.op() == "Conv2D")
    {
        tcc_assert_size_eq(node.input(), 2);

        std::string data_format = parse_attr_string(node.attr(), "data_format");
        std::string padding = parse_attr_string(node.attr(), "padding");
        dimensions strides = parse_attr_int_vec(node.attr(), "strides");
        dimensions dilations = parse_attr_int_vec(node.attr(), "dilations");
        expr input = parsed_nodes.at(node.input()[0]);
        expr filter = parsed_nodes.at(node.input()[1]);

        output = build_conv2d(
            data_format, padding, strides, dilations, input, filter);
    }
    else if (node.op() == "DepthwiseConv2dNative")
    {
        tcc_assert_size_eq(node.input(), 2);

        std::string data_format = parse_attr_string(node.attr(), "data_format");
        std::string padding = parse_attr_string(node.attr(), "padding");
        dimensions strides = parse_attr_int_vec(node.attr(), "strides");
        dimensions dilations = parse_attr_int_vec(node.attr(), "dilations");
        expr input = parsed_nodes.at(node.input()[0]);
        expr filter = parsed_nodes.at(node.input()[1]);

        output = build_depthwiseconv2dnative(
            data_format, padding, strides, dilations, input, filter);
    }
    else if (node.op() == "FusedBatchNorm")
    {
        tcc_assert_size_eq(node.input(), 5);

        float epsilon = parse_attr_float(node.attr(), "epsilon");
        std::string data_format = parse_attr_string(node.attr(), "data_format");
        expr x = parsed_nodes.at(node.input()[0]);
        expr scale = parsed_nodes.at(node.input()[1]);
        expr offset = parsed_nodes.at(node.input()[2]);
        expr mean = parsed_nodes.at(node.input()[3]);
        expr variance = parsed_nodes.at(node.input()[4]);

        output = build_fusedbatchnorm(
            epsilon, data_format, x, scale, offset, mean, variance);
    }
    else if (node.op() == "Relu6")
    {
        tcc_assert_size_eq(node.input(), 1);

        expr features = parsed_nodes.at(node.input()[0]);

        output = build_relu6(features);
    }
    else if (node.op() == "Reshape")
    {
        tcc_assert_size_eq(node.input(), 2);

        expr tensor = parsed_nodes.at(node.input()[0]);
        expr shape = parsed_nodes.at(node.input()[1]);

        output = build_reshape(tensor, shape);
    }
    else if (node.op() == "Shape")
    {
        tcc_assert_size_eq(node.input(), 1);

        expr input = parsed_nodes.at(node.input()[0]);

        output = build_shape(input);
    }
    else if (node.op() == "Softmax")
    {
        tcc_assert_size_eq(node.input(), 1);

        expr logits = parsed_nodes.at(node.input()[0]);

        output = build_softmax(logits);
    }
    else if (node.op() == "Squeeze")
    {
        tcc_assert_size_eq(node.input(), 1);

        dimensions squeeze_dims =
            parse_attr_int_vec(node.attr(), "squeeze_dims");
        expr input = parsed_nodes.at(node.input()[0]);

        output = build_squeeze(squeeze_dims, input);
    }
    else
    {
        tcc_error("unsupported tensorflow op " + node.op() + ".");
    }

    /* save core output to parsed nodes. */
    tcc_assert_not_null(output);
    tcc_assert_no_key(parsed_nodes, node.name());
    parsed_nodes.insert({ node.name(), output });
}

static void recurse_graph(
    std::string current_node_name,
    std::unordered_map<std::string, tensorflow::NodeDef>& nodes,
    std::unordered_set<std::string>& traversed,
    std::unordered_map<std::string, expr>& parsed_nodes,
    std::unordered_map<std::string, dimensions>& input_shapes)
{
    if (traversed.find(current_node_name) == traversed.end())
    {
        traversed.insert(current_node_name);

        tcc_assert_has_key(nodes, current_node_name);
        tensorflow::NodeDef current_node = nodes.at(current_node_name);

        /* recurses the inputs of the current node;
         * reaches base case when there is no inputs to the current node. */
        for (std::string input_node_name : current_node.input())
        {
            recurse_graph(
                input_node_name, nodes, traversed, parsed_nodes, input_shapes);
            tcc_assert_has_key(parsed_nodes, input_node_name);
        }

        parse_node(current_node, parsed_nodes, input_shapes);
    }
}

static expr parse_graph(
    tensorflow::GraphDef& graph,
    std::unordered_map<std::string, dimensions>& input_shapes)
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

    /* recurively traverse the tensorflow graph and
     * parse each tensorflow node into core. */
    std::unordered_set<std::string> traversed;
    std::unordered_map<std::string, expr> parsed_nodes;
    recurse_graph(output_name, nodes, traversed, parsed_nodes, input_shapes);

    tcc_assert_has_key(parsed_nodes, output_name);
    return parsed_nodes.at(output_name);
}

expr parse(const std::string input_path,
           std::unordered_map<std::string, dimensions>& input_shapes)
{
    tensorflow::GraphDef graph = load_graph(input_path);
    return parse_graph(graph, input_shapes);
}

} // namespace tcc
