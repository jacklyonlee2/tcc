#include "tcc/core/llir/llir.h"

namespace tcc {
namespace core {

/* compute function implementation. */

Pmt compute(
        std::vector<long> shape,
        std::function<Pmt(Axes)> lambda) {
    Axes axes(shape);
    Pmt ret = lambda(axes);
    ret->set_axes(axes);
    return ret;
}

/* LLIRVisitor method implementations. */

/* LLIR method implementations. */

} // namespace core
} // namespace tcc
