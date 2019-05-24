#include "tcc/core/operator.h"

#include "tcc/core/operator_registry.h"
#include "tcc/core/operator_builder.h"
#include "tcc/util/logging.h"

namespace tcc {
namespace core {

Operator::Operator(OperatorBuilder& builder) {
    // Store operator into static operator registry.
    OperatorRegistry::Register(builder.type_name_, *this);
}

} // namespace core
} // namespace tcc
