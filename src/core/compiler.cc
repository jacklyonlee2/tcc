#include "tcc/core/compiler.h"

namespace tcc {
namespace core {

CompilerBuilder& CompilerBuilder::HLIRBuilder(HLIR(*hlir_builder)(std::string input_path)) {
    CHECK(hlir_builder_ == nullptr) <<
        "HLIR builder already registered.";
    hlir_builder_ = hlir_builder;
}

Compiler::Compiler(CompilerBuilder& builder) :
    hlir_builder_(builder.hlir_builder_) {
}

Compiler& Compiler::BuildHLIR(std::string input_path) {
    CHECK(hlir_builder_ != nullptr) <<
        "HLIRBuilder is not registered.";
    hlir_ptr_ = std::make_shared<HLIR>(hlir_builder_(input_path));

    return *this;
}

Compiler& Compiler::PrintHLIR(std::string output_path) {
    CHECK(hlir_ptr_ != nullptr) <<
        "PrintHLIR must be called after BuildHLIR.";

    std::ofstream file(output_path, std::ios::trunc);
    CHECK(file) <<
        "Failed to open file at '" << output_path << "'.";

    hlir_ptr_->Print(file);
    file.close();

    return *this;
}

} // namespace core
} // namespace tcc
