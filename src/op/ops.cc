#include "tcc/core/operator.h"

using namespace tcc::core;

REGISTER_OP("Relu6")
    .Input("features")
    .Output("activations");

REGISTER_OP("DepthwiseConv2dNative")
    .Attr("dilations", Datatype::kListInt64)
    .Attr("data_format", Datatype::kString)
    .Attr("strides", Datatype::kListInt64)
    .Attr("padding", Datatype::kString)
    .Input("input")
    .Input("filter")
    .Output("output");

REGISTER_OP("FusedBatchNorm")
    .Attr("epsilon", Datatype::kScalarFloat32)
    .Attr("data_format", Datatype::kString)
    .Input("x")
    .Input("scale")
    .Input("offset")
    .Input("mean")
    .Input("variance")
    .Output("y");

REGISTER_OP("Add")
    .Input("x")
    .Input("y")
    .Output("z");

REGISTER_OP("AvgPool")
    .Attr("ksize", Datatype::kListInt64)
    .Attr("padding", Datatype::kString)
    .Attr("data_format", Datatype::kString)
    .Attr("strides", Datatype::kListInt64)
    .Input("value")
    .Output("output");

REGISTER_OP("Conv2D")
    .Attr("dilations", Datatype::kListInt64)
    .Attr("data_format", Datatype::kString)
    .Attr("strides", Datatype::kListInt64)
    .Attr("padding", Datatype::kString)
    .Input("input")
    .Input("filter")
    .Output("output");

REGISTER_OP("BiasAdd")
    .Attr("data_format", Datatype::kString)
    .Input("value")
    .Input("bias")
    .Output("output");

REGISTER_OP("Squeeze")
    .Attr("squeeze_dims", Datatype::kListInt64)
    .Input("input")
    .Output("output");

REGISTER_OP("Reshape")
    .Input("tensor")
    .Input("shape")
    .Output("output");

REGISTER_OP("Softmax")
    .Input("logits")
    .Output("output");

REGISTER_OP("Shape")
    .Input("input")
    .Output("output");
