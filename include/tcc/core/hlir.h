#ifndef TCC_HLIR_H
#define TCC_HLIR_H

#include <unordered_map>
#include <unordered_set>
#include <fstream>

#include "tcc/core/data.h"

namespace tcc {
namespace core {

class HLIR {
    public:
        class Operation;
        typedef std::shared_ptr<Operation> OperationPtr;

        class Variable {
            public:
                Variable(const std::string instance_name, Datatype datatype);
                Variable(const std::string instance_name, Data data);
                Variable(const std::string instance_name, OperationPtr prev_op);

                bool Placeholder() const;
                bool Constant() const;

                OperationPtr GetPrevOperation() const;
                std::vector<OperationPtr> GetNextOperations() const;

                const std::string instance_name_;

            private:
                Datatype datatype_ = Datatype::kUninitialized;
                Data data_;
                OperationPtr prev_op_ = nullptr;
                std::unordered_set<OperationPtr> next_ops_;

            friend class Operation;
        };

        typedef std::shared_ptr<Variable> VariablePtr;
        typedef std::weak_ptr<Variable> WeakVariablePtr;

        class Operation {
            public:
                Operation(
                        const std::string instance_name,
                        const std::string type_name,
                        std::unordered_map<std::string, Data> attr_val_map);

                std::string GetInputName(VariablePtr input_variable) const;
                std::string GetOutputName(VariablePtr output_variable) const;
                VariablePtr GetOutputVariable(unsigned int index) const;
                std::vector<VariablePtr> GetInputVariables() const;
                std::vector<VariablePtr> GetOutputVariables() const;

                static std::vector<VariablePtr> Infer(
                        OperationPtr operation,
                        std::vector<VariablePtr> input_variables);

                const std::string instance_name_;
                const std::string type_name_;

            private:
                std::unordered_map<std::string, Data> attr_val_map_;
                std::vector<std::string> input_variable_names_;
                std::vector<std::string> output_variable_names_;
                std::unordered_map<std::string, WeakVariablePtr> input_variable_map_;
                std::unordered_map<std::string, WeakVariablePtr> output_variable_map_;
        };

        HLIR(std::unordered_map<std::string, VariablePtr> variable_map,
                std::unordered_map<std::string, OperationPtr> operation_map);

        void Print(std::ofstream& stream) const;

    private:
        std::unordered_map<std::string, VariablePtr> variable_map_;
        std::unordered_map<std::string, OperationPtr> operation_map_;
};

} // namespace core
} // namespace tcc

#endif // TCC_HLIR_H
