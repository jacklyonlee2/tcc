#include "tcc/core/common/register_op.h"
#include "tcc/core/common/datatype.h"

using namespace tcc::core::common;

REGISTER_OP("Relu6")
    .Input("Input", Datatype::kTensorFloat32)
    .Output("Output", Datatype::kTensorFloat32);

REGISTER_OP("DepthwiseConv2dNative");

REGISTER_OP("FusedBatchNorm");

REGISTER_OP("Add");

REGISTER_OP("AvgPool");

REGISTER_OP("Conv2D");

REGISTER_OP("BiasAdd");

REGISTER_OP("Squeeze");

REGISTER_OP("Reshape");

REGISTER_OP("Softmax");

REGISTER_OP("Shape");
