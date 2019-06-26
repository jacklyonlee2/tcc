#include "tcc/core/operator.h"
#include "tcc/core/context.h"
#include "tcc/core/util.h"

using namespace tcc::core;

REGISTER_OP("Relu6")
    .Input("features")
    .Output("activations")
    .Kernel([](KernelContext& ctx) {
            });

REGISTER_OP("DepthwiseConv2dNative")
    .Attr("dilations", Datatype::kTensorI64)
    .Attr("data_format", Datatype::kScalarSTR)
    .Attr("strides", Datatype::kTensorI64)
    .Attr("padding", Datatype::kScalarSTR)
    .Input("input")
    .Input("filter")
    .Output("output")
    .Kernel([](KernelContext& ctx) {
            });

REGISTER_OP("FusedBatchNorm")
    .Attr("epsilon", Datatype::kScalarFP32)
    .Attr("data_format", Datatype::kScalarSTR)
    .Input("x")
    .Input("scale")
    .Input("offset")
    .Input("mean")
    .Input("variance")
    .Output("y")
    .Kernel([](KernelContext& ctx) {
            });

REGISTER_OP("Add")
    .Input("x")
    .Input("y")
    .Output("z")
    .Kernel([](KernelContext& ctx) {
            });

REGISTER_OP("AvgPool")
    .Attr("ksize", Datatype::kTensorI64)
    .Attr("padding", Datatype::kScalarSTR)
    .Attr("data_format", Datatype::kScalarSTR)
    .Attr("strides", Datatype::kTensorI64)
    .Input("value")
    .Output("output")
    .Kernel([](KernelContext& ctx) {
            });

REGISTER_OP("Conv2D")
    .Attr("dilations", Datatype::kTensorI64)
    .Attr("data_format", Datatype::kScalarSTR)
    .Attr("strides", Datatype::kTensorI64)
    .Attr("padding", Datatype::kScalarSTR)
    .Input("input")
    .Input("filter")
    .Output("output")
    .Kernel([](KernelContext& ctx) {
            auto dilations = ctx.GetAttr("dilations").GetkTensorI64();
            auto data_format = ctx.GetAttr("data_format").GetkScalarSTR();
            auto strides = ctx.GetAttr("strides").GetkTensorI64();
            auto padding = ctx.GetAttr("padding").GetkScalarSTR();

            CHECK(dilations.size() == 4 && dilations == Literal(1l,1l,1l,1l)) <<
                "Conv2D does not support dilation.";
            CHECK(data_format == "NHWC") <<
                "Conv2D only supports NHWC format.";
            CHECK(padding == "SAME") <<
                "Conv2D only supports SAME padding";

            auto input = ctx.GetInput("input");
            auto filter = ctx.GetInput("filter");
        });

REGISTER_OP("BiasAdd")
    .Attr("data_format", Datatype::kScalarSTR)
    .Input("value")
    .Input("bias")
    .Output("output")
    .Kernel([](KernelContext& ctx) {
            });

REGISTER_OP("Squeeze")
    .Attr("squeeze_dims", Datatype::kTensorI64)
    .Input("input")
    .Output("output")
    .Kernel([](KernelContext& ctx) {
            });

REGISTER_OP("Reshape")
    .Input("tensor")
    .Input("shape")
    .Output("output")
    .Kernel([](KernelContext& ctx) {
            });

REGISTER_OP("Softmax")
    .Input("logits")
    .Output("output")
    .Kernel([](KernelContext& ctx) {
            });

REGISTER_OP("Shape")
    .Input("input")
    .Output("output")
    .Kernel([](KernelContext& ctx) {
            });
