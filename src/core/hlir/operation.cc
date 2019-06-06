#include "tcc/core/hlir/operation.h"

#include "tcc/util/logging.h"

namespace tcc {
namespace core {
namespace hlir {

Operation::Operation(const std::string instance_name, const common::Operator& op) :
    instance_name_(instance_name),
    attrs_(op.attr_val_map_),
    input_list_(op.input_list_),
    output_list_(op.output_list_) {
    // Initialize input_variable_map_ with empty weak ptr
    for (std::string input_name : op.input_list_) {
        input_variable_map_.insert({input_name, WeakVariablePtr()});
    }

    // Initialize output_variable_map_ with empty weak ptr
    for (std::string output_name : op.output_list_) {
        output_variable_map_.insert({output_name, WeakVariablePtr()});
    }
}

} // namespace hlir
} // namespace core
} // namespace tcc
