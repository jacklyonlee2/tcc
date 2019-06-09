#include "tcc/core/compiler.h"

#include <fstream>

#include "tcc/core/context.h"
#include "tcc/core/logging.h"

namespace tcc {
namespace core {

CompilerBuilder& CompilerBuilder::Parser(void(*parser)(ParserContext&)) {
    CHECK(parser_ == nullptr) << "Parser already registered.";

    parser_ = parser;
}

Compiler::Compiler(CompilerBuilder& builder) {
    CHECK_NOTNULL(builder.parser_);
    parser_ = builder.parser_;
}

Compiler& Compiler::ParseInput(std::string input_path) {
    ParserContext parser_ctx(input_path);
    parser_(parser_ctx);
    HLIR hlir = parser_ctx.BuildHLIR();
    hlir_ptr_ = std::make_shared<HLIR>(hlir);

    return *this;
}

Compiler& Compiler::PrintHLIR(std::string output_path) {
    CHECK_NOTNULL(hlir_ptr_); // ParseInput must be called first
    std::ofstream file(output_path, std::ios::trunc);
    CHECK(file) << "Failed to open file at '" << output_path << "'.";
    hlir_ptr_->Print(file);
    file.close();

    return *this;
}

Compiler& Compiler::OptimizeHLIR() {
    CHECK_NOTNULL(hlir_ptr_); // ParseInput must be called first

    return *this;
}

Compiler& Compiler::HLIRtoLLIR() {
    CHECK_NOTNULL(hlir_ptr_); // ParseInput must be called first

    return *this;
}

Compiler& Compiler::PrintLLIR(std::string output_path) {
    return *this;
}

Compiler& Compiler::OptimizeLLIR() {
    return *this;
}

Compiler& Compiler::LLIRtoCGEN() {
    return *this;
}

Compiler& Compiler::PrintCGEN(std::string output_path) {
    return *this;
}

Compiler& Compiler::OptimizeCGEN() {
    return *this;
}

void Compiler::GenerateOutput(std::string output_path) {
}

} // namespace core
} // namespace tcc
