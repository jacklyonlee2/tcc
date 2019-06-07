#include "tcc/core/operator_registry.h"

namespace tcc {
namespace core {

void OperatorRegistry::Register(std::string type_name, Operator& op) {
    // This function is called at static initialization time, NATIVE_CHECK is used instead of CHECK.
    // Because static initalization order is random, Register could be called before registry_ is initalized.
    // The following lines ensure registry_ is initialize before any op registration.
    if (registry_ == nullptr) {
        registry_ = std::shared_ptr<std::unordered_map<std::string, Operator>>(
                new std::unordered_map<std::string, Operator>());
    }

    NATIVE_CHECK_KEY_NOT_IN_MAP(type_name, *registry_);
    NATIVE_CHECK(!type_name.empty(), "Operator type name can not be empty.");
    registry_->insert({type_name, op});
}

std::shared_ptr<std::unordered_map<std::string, Operator>> OperatorRegistry::registry_;

} // namespace core
} // namespace tcc
