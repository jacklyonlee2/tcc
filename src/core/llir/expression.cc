#include "tcc/core/llir/expression.h"

#include "tcc/core/llir/llir.h"

namespace tcc {
namespace core {

/* Override LLIR Primitive accept method. */

#define IMPLEMENT_ACCEPT(type) \
    template<> void Expression<type>::accept(LLIRVisitor *v) const { \
        v->visit(expr::downcast<type>(shared_from_this())); \
    }

IMPLEMENT_ACCEPT(expr::Range)

#undef IMPLEMENT_ACCEPT

namespace expr {

Expr Range::make(long bound_) {
    CHECK(bound_ > 0) << "Max must be positive.";

    std::shared_ptr<Range> expr(new Range);
    expr->value_type = ValueType::INDICES;
    expr->range = std::pair<long,long>({0l, bound_});
    return expr;
}

} // namespace expr
} // namespace core
} // namespace tcc
