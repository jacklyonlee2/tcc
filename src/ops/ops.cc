#include "tcc/core/operator/register_op.h"

REGISTER_OP("Test")
    .Attr("attr")
    .Input("i1")
    .Output("o1");
