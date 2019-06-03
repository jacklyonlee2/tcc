#include "tcc/core/common/operator.h"

#include "tcc/core/common/operator_registry.h"
#include "tcc/core/common/operator_builder.h"
#include "tcc/util/logging.h"

namespace tcc {
namespace core {
namespace common {

Operator::Operator(OperatorBuilder builder) {
    // This function is called at static initialization time, NATIVE_CHECK is used instead of CHECK.
    // Store operator into static operator registry.
    OperatorRegistry::Register(builder.type_name_, *this);
}

} // namespace common
} // namespace core
} // namespace tcc
