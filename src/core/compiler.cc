#include "tcc/core/compiler.h"

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

void Compiler::Compile(std::string input_path) {
    // Parse input file
    ParserContext parser_ctx(input_path);
    parser_(parser_ctx);
}

void Compiler::VisualizeHLIR(std::string output_path) {
}

} // namespace core
} // namespace tcc
