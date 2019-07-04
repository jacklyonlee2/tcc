#include "tcc/core/llir/expression.h"

namespace tcc {
namespace core {
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
