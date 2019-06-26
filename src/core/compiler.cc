#include "tcc/core/compiler.h"

namespace tcc {
namespace core {

CompilerBuilder& CompilerBuilder::HLIRBuilder(
        HLIR(*hlir_builder)(
            std::string input_path,
            std::unordered_map<std::string, std::vector<long>>)) {
    CHECK(hlir_builder_ == nullptr) <<
        "HLIRbuilder is already registered.";
    hlir_builder_ = hlir_builder;

    return *this;
}

Compiler::Compiler(CompilerBuilder& builder) :
    hlir_builder_(builder.hlir_builder_) {
}

Compiler& Compiler::BuildHLIR(
        std::string input_path,
        std::unordered_map<std::string, std::vector<long>> input_shapes) {
    CHECK(hlir_builder_ != nullptr) <<
        "HLIRBuilder is not registered.";
    CHECK(hlir_ptr_ == nullptr) <<
        "HLIR is already built.";
    hlir_ptr_ = std::make_shared<HLIR>(hlir_builder_(input_path, input_shapes));

    return *this;
}

Compiler& Compiler::PrintHLIR(std::string output_path) {
    CHECK(hlir_ptr_ != nullptr) <<
        "PrintHLIR must be called after BuildHLIR.";

    std::ofstream file(output_path, std::ios::trunc);
    CHECK(file) <<
        "Failed to open file at" QUOTE_MSG(output_path) ".";

    hlir_ptr_->Print(file);
    file.close();

    return *this;
}

Compiler& Compiler::BuildLLIR() {
    CHECK(hlir_ptr_ != nullptr) <<
        "BuildLLIR must be called after BuildHLIR.";
    CHECK(llir_ptr_ == nullptr) <<
        "LLIR is already built.";
    llir_ptr_ = std::make_shared<LLIR>(hlir_ptr_->Lower());

    return *this;
}

} // namespace core
} // namespace tcc
