#ifndef TCC_COMPILER_H
#define TCC_COMPILER_H

#include <vector>
#include <memory>

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

        Compiler& ParseInput(std::string input_path);
        Compiler& PrintHLIR(std::string output_path);
        Compiler& OptimizeHLIR();

        Compiler& HLIRtoLLIR();
        Compiler& PrintLLIR(std::string output_path);
        Compiler& OptimizeLLIR();

        Compiler& LLIRtoCGEN();
        Compiler& PrintCGEN(std::string output_path);
        Compiler& OptimizeCGEN();

        void GenerateOutput(std::string output_path);

    private:
        void(*parser_)(ParserContext&);

        std::shared_ptr<HLIR> hlir_ptr_ = nullptr;
        std::shared_ptr<LLIR> llir_ptr_ = nullptr;
};

} // namespace core
} // namespace tcc

#define BUILD_COMPILER(name) \
    ::tcc::core::Compiler name = ::tcc::core::CompilerBuilder()

#endif // TCC_COMPILER_H
