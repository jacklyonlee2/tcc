#include "tcc/core/common/operator.h"

#include "tcc/core/common/operator_registry.h"
#include "tcc/core/common/operator_builder.h"
#include "tcc/util/logging.h"

namespace tcc {
namespace core {
namespace common {

Operator::Operator(OperatorBuilder builder) :
    type_name_(builder.type_name_),
    attr_type_map_(builder.attr_type_map_),
    input_type_map_(builder.input_type_map_),
    output_type_map_(builder.output_type_map_) {
    // Store operator into static operator registry.
    OperatorRegistry::Register(builder.type_name_, *this);
}

} // namespace common
} // namespace core
} // namespace tcc
