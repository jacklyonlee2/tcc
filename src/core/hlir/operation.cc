#include "tcc/core/hlir/operation.h"

namespace tcc {
namespace core {
namespace hlir {

Operation::Operation(std::string instance_name, common::Operator op) :
    instance_name_(instance_name) {
}

} // namespace hlir
} // namespace core
} // namespace tcc
