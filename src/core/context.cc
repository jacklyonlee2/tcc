#include "tcc/core/context.h"

#include "tcc/core/logging.h"

namespace tcc {
namespace core {

ParserContext::ParserContext(const std::string input_path) :
    input_path_(input_path) {}

bool ParserContext::OperatorExists(const std::string type) {
    CHECK_NOTNULL(OperatorRegistry::registry_);
    return OperatorRegistry::registry_->find(type) != OperatorRegistry::registry_->end();
}

Operator ParserContext::OperatorInstantiate(const std::string type) {
    CHECK_NOTNULL(OperatorRegistry::registry_);
    CHECK_KEY_IN_MAP(type, *OperatorRegistry::registry_);
    return OperatorRegistry::registry_->at(type);
}

void ParserContext::AddVariable(const std::string name, HLIR::Variable variable) {
}

void ParserContext::AddOperation(const std::string name, HLIR::Operation operation,
        std::vector<std::string> input_variable_names,
        std::vector<std::string> output_variable_names) {
}

} // namespace core
} // namespace tcc
