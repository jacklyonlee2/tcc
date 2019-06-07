#ifndef TCC_COMPILER_H
#define TCC_COMPILER_H

#include <vector>

#include "tcc/core/context.h"

namespace tcc {
namespace core {

class Compiler;

class CompilerBuilder {
    public:
        CompilerBuilder& Parser(void(*parser)(ParserContext&));
        //CompilerBuilder& HLIRPasses(std::vector<void(*)(HLIRPassContext&)> hlir_passes);
        //CompilerBuilder& LLIRPasses(std::vector<void(*)(LLIRPassContext&)> llir_passes);
        //CompilerBuilder& CGENPasses(std::vector<void(*)(CGENPassContext&)> cgen_passes);

    private:
        void(*parser_)(ParserContext&) = nullptr;

    friend class Compiler;
};

class Compiler {
    public:
        Compiler(CompilerBuilder& builder);

        void Compile(std::string input_path);
        void VisualizeHLIR(std::string output_path);

    private:
        void(*parser_)(ParserContext&);
};

} // namespace core
} // namespace tcc

#define BUILD_COMPILER(name) \
    ::tcc::core::Compiler name = ::tcc::core::CompilerBuilder()

#endif // TCC_COMPILER_H
