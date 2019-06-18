#include "tcc/core/operator.h"
#include "tcc/core/context.h"

using namespace tcc::core;

REGISTER_OP("Relu6")
    .Input("features")
    .Output("activations")
    .Kernel([](KernelContext& ctx){
            });

REGISTER_OP("DepthwiseConv2dNative")
    .Attr("dilations", Datatype::kVectorI64)
    .Attr("data_format", Datatype::kScalarSTR)
    .Attr("strides", Datatype::kVectorI64)
    .Attr("padding", Datatype::kScalarSTR)
    .Input("input")
    .Input("filter")
    .Output("output");

REGISTER_OP("FusedBatchNorm")
    .Attr("epsilon", Datatype::kScalarFP32)
    .Attr("data_format", Datatype::kScalarSTR)
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
    .Attr("ksize", Datatype::kVectorI64)
    .Attr("padding", Datatype::kScalarSTR)
    .Attr("data_format", Datatype::kScalarSTR)
    .Attr("strides", Datatype::kVectorI64)
    .Input("value")
    .Output("output");

REGISTER_OP("Conv2D")
    .Attr("dilations", Datatype::kVectorI64)
    .Attr("data_format", Datatype::kScalarSTR)
    .Attr("strides", Datatype::kVectorI64)
    .Attr("padding", Datatype::kScalarSTR)
    .Input("input")
    .Input("filter")
    .Output("output");

REGISTER_OP("BiasAdd")
    .Attr("data_format", Datatype::kScalarSTR)
    .Input("value")
    .Input("bias")
    .Output("output");

REGISTER_OP("Squeeze")
    .Attr("squeeze_dims", Datatype::kVectorI64)
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
