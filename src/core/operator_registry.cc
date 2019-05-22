#include "tcc/core/operator_registry.h"

#include "glog/logging.h"

namespace tcc {
namespace core {

std::unordered_map<std::string, Operator> OperatorRegistry::registry_ =
    std::unordered_map<std::string, Operator>();

void OperatorRegistry::Register(std::string type_name, Operator op) {
    LOG(INFO) << "Registered " << type_name;
    CHECK(registry_.find(type_name) == registry_.end()) <<
        "Operator type " << type_name << " is already registered.";
    registry_[type_name] = op;
}

bool OperatorRegistry::GetTest(std::string type_name) {
    return registry_.find(type_name) != registry_.end();
}

} // namespace core
} // namespace tcc
