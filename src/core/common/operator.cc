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
    input_list_(builder.input_list_),
    output_list_(builder.output_list_) {
    // Initialize attr_val_map_ with nullptr
    for (std::pair<std::string, Datatype> attr_type_kv : builder.attr_type_map_) {
        attr_val_map_.insert({attr_type_kv.first, nullptr});
    }

    // Store operator into static operator registry
    OperatorRegistry::Register(builder.type_name_, *this);
}

void Operator::SetAttr(std::string attr_name, Data attr_val) {
    CHECK_KEY_IN_MAP(attr_name, attr_type_map_);
    CHECK_KEY_IN_MAP(attr_name, attr_val_map_);
    CHECK(attr_type_map_.at(attr_name) == attr_val.GetType()) <<
        "Actual attr type does not agree with declared attr type.";
    CHECK(attr_val_map_.at(attr_name) == nullptr) <<
        "Attr value for '" << attr_name << "' has already been set.";

    attr_val_map_.at(attr_name) = std::make_shared<Data>(attr_val);
}

} // namespace common
} // namespace core
} // namespace tcc
