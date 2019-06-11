#ifndef TCC_CONTEXT_H
#define TCC_CONTEXT_H

#include "tcc/core/operator.h"
#include "tcc/core/operator_registry.h"
#include "tcc/core/hlir.h"
#include "tcc/core/llir.h"

namespace tcc {
namespace core {

class Compiler;

class ParserContext {
    public:
        typedef std::pair<
            std::pair<std::string, unsigned int>,
            std::pair<std::string, unsigned int>> Edge;

        ParserContext(const std::string input_path);

        bool OperatorExists(const std::string type);
        Operator OperatorInstantiate(const std::string type);

        void AddPlaceholder(const std::string name, Datatype datatype);
        void AddConstant(const std::string name, Data data);
        void AddOperation(const std::string name, Operator op);
        void AddEdge(const std::string src_name, const unsigned int output_index,
                const std::string dst_name,  const unsigned int input_index);

    private:
        std::unordered_map<std::string, Datatype> placeholder_map_;
        std::unordered_map<std::string, Data> constant_map_;
        std::unordered_map<std::string, Operator> operation_map_;
        std::vector<Edge> edges_;

        HLIR BuildHLIR();

    public:
        const std::string input_path_;

    friend class Compiler;
};

} // namespace core
} // namespace tcc

#endif // TCC_CONTEXT_H
