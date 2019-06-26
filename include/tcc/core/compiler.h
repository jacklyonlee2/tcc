#ifndef TCC_COMPILER_H
#define TCC_COMPILER_H

#include <vector>
#include <memory>
#include <fstream>

#include "tcc/core/hlir.h"

namespace tcc {
namespace core {

class Compiler;

class CompilerBuilder {
    public:
        CompilerBuilder& HLIRBuilder(
                HLIR(*hlir_builder)(
                    std::string,
                    std::unordered_map<std::string, std::vector<long>>));

    private:
        HLIR(*hlir_builder_)(
                std::string,
                std::unordered_map<std::string, std::vector<long>>) = nullptr;

    friend class Compiler;
};

class Compiler {
    public:
        Compiler(CompilerBuilder& builder);

        Compiler& BuildHLIR(
                std::string input_path,
                 std::unordered_map<std::string, std::vector<long>> input_shapes);
        Compiler& PrintHLIR(std::string output_path);
        Compiler& BuildLLIR();

    private:
        HLIR(*hlir_builder_)(
                std::string,
                std::unordered_map<std::string, std::vector<long>>);
        std::shared_ptr<HLIR> hlir_ptr_ = nullptr;
        std::shared_ptr<LLIR> llir_ptr_ = nullptr;
};

} // namespace core
} // namespace tcc

#define BUILD_COMPILER(name) \
    ::tcc::core::Compiler name = ::tcc::core::CompilerBuilder()

#endif // TCC_COMPILER_H
