#include "tcc/core/operator/operator_registry.h"

#include "tcc/util/logging.h"

namespace tcc {
namespace core {
namespace op {

Operator OperatorRegistry::Instantiate(std::string type_name) {
    // registry_ could be unintialized if no operator registered or
    // source code containing op registrations is not linked correctly.
    CHECK_NOTNULL(registry_);
    CHECK_KEY_IN_MAP(type_name, *registry_);

    return registry_->at(type_name);
}

void OperatorRegistry::Register(std::string type_name, Operator& op) {
    // Because static initalization order is random, Register could be called before registry_ is initalized.
    // The following lines ensure registry_ is initialize before any op registration.
    if (registry_ == nullptr) {
        registry_ = RegistryPtr(new std::unordered_map<std::string, Operator>());
    }

    CHECK_KEY_NOT_IN_MAP(type_name, *registry_);
    registry_->insert({type_name, op});
}

OperatorRegistry::RegistryPtr OperatorRegistry::registry_;

} // namespace op
} // namespace core
} // namespace tcc
