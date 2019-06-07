#ifndef TCC_CONTEXT_H
#define TCC_CONTEXT_H

#include "tcc/core/operator.h"
#include "tcc/core/operator_registry.h"
#include "tcc/core/hlir.h"

namespace tcc {
namespace core {

class ParserContext {
    public:
        ParserContext(const std::string input_path);

        bool OperatorExists(const std::string type);
        Operator OperatorInstantiate(const std::string type);

        void AddVariable(const std::string name, HLIR::Variable variable);
        void AddOperation(const std::string name, HLIR::Operation operation,
                std::vector<std::string> input_variable_names,
                std::vector<std::string> output_variable_names);

    private:
        HLIR hlir_;

    public:
        const std::string input_path_;
};

} // namespace core
} // namespace tcc

#endif // TCC_CONTEXT_H
