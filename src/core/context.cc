#include "tcc/core/context.h"

namespace tcc {
namespace core {

KernelContext::KernelContext(HLIR::OperationPtr operation) :
    operation_(operation) {
    // Initalize input fragment map
    CHECK_NOTNULL(operation);
    HLIR::VariablePtr input_variable;
    for (std::string input_name : operation->GetInputNames()) {
        input_variable = operation->GetInputVariable(input_name);
        input_fragment_map_.insert({input_name,
                std::make_shared<LLIR::Fragment>(
                        LLIR::Fragment(
                            input_variable->instance_name_,
                            input_variable->GetData()))});
    }

    // Initalize output fragment map
    for (std::string output_name : operation->GetOutputNames()) {
        output_fragment_map_.insert({output_name, nullptr});
    }
}

Data KernelContext::GetAttr(std::string attr_name) const {
    return operation_->GetAttr(attr_name);
}

LLIR::FragmentPtr KernelContext::GetInput(std::string input_name) const {
    CHECK_KEY_IN_MAP(input_name, input_fragment_map_) <<
        KEY_NOT_FOUND_MSG(input_name, input_fragment_map_) JOIN_MSG(for)
        HLIR_OPERATION_IDENTIFIER(*operation_);
    return input_fragment_map_.at(input_name);
}

void KernelContext::SetOutput(
        std::string output_name, LLIR::FragmentPtr fragment) {
    CHECK_KEY_IN_MAP(output_name, output_fragment_map_) <<
        KEY_NOT_FOUND_MSG(output_name, output_fragment_map_) JOIN_MSG(for)
        HLIR_OPERATION_IDENTIFIER(*operation_);
    CHECK(output_fragment_map_.at(output_name) == nullptr) <<
        "Output" QUOTE_MSG(output_name) "is already set" JOIN_MSG(for)
        HLIR_OPERATION_IDENTIFIER(*operation_);
    output_fragment_map_.at(output_name) = fragment;
}

} // namespace core
} // namespace tcc
