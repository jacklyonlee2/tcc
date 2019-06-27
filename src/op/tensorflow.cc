#include <math.h>

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

            CHECK(dilations == Literal(1l,1l,1l,1l)) <<
                "Conv2D does not support dilation.";
            CHECK(strides.size() == 4 &&
                    strides[0] == 1 && strides[3] == 1);
            CHECK(data_format == "NHWC") <<
                "Conv2D only supports NHWC format.";
            CHECK(padding == "SAME") <<
                "Conv2D only supports SAME padding";

            auto I = ctx.GetInput("input");
            auto F = ctx.GetInput("filter");

            auto input_shape = I->GetShape();
            auto filter_shape = F->GetShape();

            CHECK(input_shape.size() == 4);
            CHECK(filter_shape.size() == 4);

            long i_n, i_h, i_w, i_c, f_h, f_w, f_c, f_n;
            i_n = input_shape[0];
            i_h = input_shape[1];
            i_w = input_shape[2];
            i_c = input_shape[3];
            f_h = filter_shape[0];
            f_w = filter_shape[1];
            f_c = filter_shape[2];
            f_n = filter_shape[3];

            CHECK(f_h % 2 == 1 && f_w % 2 == 1);

            long str_h, str_w, pad_h, pad_w;
            pad_h = f_h / 2;
            pad_w = f_w / 2;
            str_h = strides[1];
            str_w = strides[2];

            CHECK(i_n == 1 && i_c == f_c && f_h <= i_h && f_w <= i_w);

            long o_n, o_h, o_w, o_c;
            o_n = i_n;
            o_h = floor(static_cast<double>(i_h+pad_h*2-f_h)/str_h + 1);
            o_w = floor(static_cast<double>(i_w+pad_w*2-f_w)/str_w + 1);
            o_c = f_n;
            /*
            auto M = LLIR::Lambda({i_n, o_h, o_w, f_h, f_w, f_c}, [](LLIR::Iterators i) {
                        return LLIR::Select(
                                LLIR::All({
                                    i[1]*str_h+i[3]-pad_h*2 >= 0,
                                    i[1]*str_h+i[3]-pad_h*2 < i_h,
                                    i[2]*str_w+i[4]-pad_w*2 >= 0,
                                    i[2]*str_w+i[4]-pad_w*2 < i_w}),
                                I(
                                    i[0],
                                    i[1]*str_h+i[3]-pad_h*2,
                                    i[2]*str_w+i[4]-pad_w*2,
                                    i[5]),
                                Data::kScalarFP32(0.0f));
                    });

            // Reduce axes
            LLIR::Iterator f_h_i(0, f_h);
            LLIR::Iterator f_w_i(0, f_w);
            LLIR::Iterator f_c_i(0, f_c);
            auto O = LLIR::Lambda({o_n, o_h, o_w, o_c}, [](LLIR::Iterators i) {
                        return LLIR::Multiply(
                                M(i[0], i[1], i[2], f_h_i, f_w_i, f_c_i),
                                F(f_h_i, f_w_i, f_c_i, i[3]),
                                {f_h_i, f_w_i, f_c_i});
                    });

            ctx.SetOutput("output", O); */
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
