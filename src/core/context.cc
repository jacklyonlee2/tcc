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
    CHECK_KEY_IN_MAP(type, *OperatorRegistry::registry_) <<
        "Operator type '" << type << "' is not found in registry_.";
    return OperatorRegistry::registry_->at(type);
}

void ParserContext::AddPlaceholder(const std::string name, Datatype datatype) {
    CHECK(datatype != Datatype::kUninitialized) <<
        "Placeholder data type can not be 'kUninitialized'.";
}

void ParserContext::AddConstant(const std::string name, Data data) {
    CHECK(!data.IsEmpty()) << "Constant data can not be uninitialized.";
}

void ParserContext::AddOperation(const std::string name, Operator op) {
}

void ParserContext::AddEdge(const std::string src_name, const unsigned int output_index,
        const std::string dst_name,  const unsigned int input_index) {
}

HLIR ParserContext::BuildHLIR() {
    return HLIR();
}

} // namespace core
} // namespace tcc
