#include "tcc/core/common/operator.h"

#include "tcc/core/common/operator_registry.h"
#include "tcc/core/common/operator_builder.h"
#include "tcc/util/logging.h"

namespace tcc {
namespace core {
namespace common {

Operator::Operator(OperatorBuilder builder) {
    // Store operator into static operator registry.
    OperatorRegistry::Register(builder.type_name_, *this);
}

} // namespace common
} // namespace core
} // namespace tcc
