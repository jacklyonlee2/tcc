#include "tcc/core/llir/llir.h"

namespace tcc {
namespace core {

/* compute function implementation. */

Expr compute(
        std::vector<long> shape,
        std::function<Expr(Axes)> lambda) {
    /* Construct Axes from output shape. */
    Axes axes;
    for (long dim : shape) {
        axes.push_back(expr::Range::make(0, dim));
    }

    Expr output = lambda(axes);
    output->data_desc.set_shape(shape);
    return output;
}

/* LLIRVisitor method implementations. */

/* LLIR method implementations. */

} // namespace core
} // namespace tcc
