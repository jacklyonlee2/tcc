#include "tcc/core/operator.h"

using namespace tcc::core;

REGISTER_OP("Relu6")
    .Input("input")
    .Output("output");

REGISTER_OP("DepthwiseConv2dNative")
    .Attr("dilations", Datatype::kListInt64)
    .Attr("data_format", Datatype::kString)
    .Attr("strides", Datatype::kListInt64)
    .Attr("padding", Datatype::kString);

REGISTER_OP("FusedBatchNorm")
    .Attr("epsilon", Datatype::kScalarFloat32)
    .Attr("data_format", Datatype::kString);

REGISTER_OP("Add");

REGISTER_OP("AvgPool")
    .Attr("ksize", Datatype::kListInt64)
    .Attr("padding", Datatype::kString)
    .Attr("data_format", Datatype::kString)
    .Attr("strides", Datatype::kListInt64)
    .Input("input")
    .Output("output");

REGISTER_OP("Conv2D")
    .Attr("dilations", Datatype::kListInt64)
    .Attr("data_format", Datatype::kString)
    .Attr("strides", Datatype::kListInt64)
    .Attr("padding", Datatype::kString);

REGISTER_OP("BiasAdd")
    .Attr("data_format", Datatype::kString);

REGISTER_OP("Squeeze")
    .Attr("squeeze_dims", Datatype::kListInt64);

REGISTER_OP("Reshape");

REGISTER_OP("Softmax");

REGISTER_OP("Shape");