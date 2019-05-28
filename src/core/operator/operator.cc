#include "tcc/core/operator/operator.h"

#include "tcc/core/operator/operator_registry.h"
#include "tcc/core/operator/operator_builder.h"
#include "tcc/util/logging.h"

namespace tcc {
namespace core {
namespace op {

Operator::Operator(OperatorBuilder& builder) {
    // Store operator into static operator registry.
    OperatorRegistry::Register(builder.type_name_, *this);
}

} // namespace op
} // namespace core
} // namespace tcc
