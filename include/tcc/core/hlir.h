#ifndef TCC_HLIR_H
#define TCC_HLIR_H

#include <unordered_map>
#include <fstream>

#include "tcc/core/data.h"
#include "tcc/core/operator.h"
#include "tcc/core/llir.h"

namespace tcc {
namespace core {

class HLIR {
    public:
        class Variable {
            public:
                Variable(const std::string prev_op_name,
                        const std::vector<std::string> next_op_names); // Intermidiates
                Variable(Data data, const std::vector<std::string> next_op_names); // Constants
                Variable(Datatype datatype, const std::vector<std::string> next_op_names); // Placeholders

            private:
                Datatype datatype_ = Datatype::kUninitialized;
                Data data_;

                std::string prev_op_name_;
                std::vector<std::string> next_op_names_;

            friend class HLIR;
        };

        class Operation {
            public:
                Operation(const std::unordered_map<std::string, Data> attrs,
                        const std::unordered_map<std::string, std::string> input_variable_map,
                        const std::unordered_map<std::string, std::string> output_variable_map);

            private:
                std::unordered_map<std::string, Data> attrs_;
                std::unordered_map<std::string, std::string> input_variable_map_;
                std::unordered_map<std::string, std::string> output_variable_map_;

            friend class HLIR;
        };

        void AddVariable(std::string name, Variable variable);
        void AddOperation(std::string name, Operation operation);

        bool IsValid() const;
        void Print(std::ofstream& stream) const;
        LLIR Lower() const;

    private:
        std::unordered_map<std::string, Variable> variable_map_;
        std::unordered_map<std::string, Operation> operation_map_;
};

} // namespace core
} // namespace tcc

#endif // TCC_HLIR_H
