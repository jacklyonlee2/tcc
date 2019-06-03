#include "tcc/core/common/operator_registry.h"

#include <assert.h>

#include "tcc/util/logging.h"

namespace tcc {
namespace core {
namespace common {

bool OperatorRegistry::Registered(const std::string type_name) {
    // registry_ could be unintialized if no operator registered or
    // source code containing op registrations is not linked correctly.
    CHECK_NOTNULL(registry_);

    return registry_->find(type_name) != registry_->end();
}

Operator OperatorRegistry::Instantiate(const std::string type_name) {
    // registry_ could be unintialized if no operator registered or
    // source code containing op registrations is not linked correctly.
    CHECK_NOTNULL(registry_);
    CHECK_KEY_IN_MAP(type_name, *registry_);

    return registry_->at(type_name);
}

void OperatorRegistry::Register(std::string type_name, Operator& op) {
    // This function is called at static initialization time, glog can not be used.
    // Because static initalization order is random, Register could be called before registry_ is initalized.
    // The following lines ensure registry_ is initialize before any op registration.
    if (registry_ == nullptr) {
        registry_ = RegistryPtr(new std::unordered_map<std::string, Operator>());
    }

    // Check type_name is not already defined in registry.
    assert(registry_->find(type_name) == registry_->end());
    registry_->insert({type_name, op});
}

OperatorRegistry::RegistryPtr OperatorRegistry::registry_;

} // namespace common
} // namespace core
} // namespace tcc
