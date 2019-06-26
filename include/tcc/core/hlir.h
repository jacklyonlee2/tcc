#ifndef TCC_HLIR_H
#define TCC_HLIR_H

#include <unordered_map>
#include <unordered_set>
#include <fstream>

#include "tcc/core/data.h"
#include "tcc/core/llir.h"

namespace tcc {
namespace core {

class KernelContext;

class HLIR {
    public:
        class Operation;
        typedef std::shared_ptr<Operation> OperationPtr;

        class Variable {
            public:
                Variable(const std::string instance_name, Data data);
                Variable(const std::string instance_name, OperationPtr prev_op);

                bool Terminal() const { return prev_op_ == nullptr; }
                Data GetData() const { return data_; }
                OperationPtr GetPrevOperation() const;
                std::vector<OperationPtr> GetNextOperations() const;

                const std::string instance_name_;
                const Data data_;

            private:
                OperationPtr prev_op_;
                std::unordered_set<OperationPtr> next_ops_;

            friend class HLIR;
        };

        typedef std::shared_ptr<Variable> VariablePtr;
        typedef std::weak_ptr<Variable> WeakVariablePtr;

        class Operation {
            public:
                Operation(
                        const std::string instance_name,
                        const std::string type_name,
                        std::unordered_map<std::string, Data> attr_val_map);

                Data GetAttr(std::string attr_name) const;
                std::vector<std::string> GetInputNames() const { return input_variable_names_; }
                std::vector<std::string> GetOutputNames() const { return output_variable_names_; }
                std::string GetInputName(VariablePtr input_variable) const;
                std::string GetOutputName(VariablePtr output_variable) const;
                VariablePtr GetInputVariable(std::string input_name) const;
                VariablePtr GetInputVariable(unsigned int input_index) const;
                VariablePtr GetOutputVariable(std::string output_name) const;
                VariablePtr GetOutputVariable(unsigned int output_index) const;
                std::vector<VariablePtr> GetInputVariables() const;
                std::vector<VariablePtr> GetOutputVariables() const;

                const std::string instance_name_;
                const std::string type_name_;

            private:
                std::unordered_map<std::string, Data> attr_val_map_;
                std::vector<std::string> input_variable_names_;
                std::vector<std::string> output_variable_names_;
                void(*kernel_)(KernelContext&);

                std::unordered_map<std::string, WeakVariablePtr> input_variable_map_;
                std::unordered_map<std::string, WeakVariablePtr> output_variable_map_;

            friend class HLIR;
        };

        static std::vector<VariablePtr> Connect(
                OperationPtr operation, std::vector<VariablePtr> input_variables);

        HLIR(std::unordered_map<std::string, VariablePtr> variable_map,
                std::unordered_map<std::string, OperationPtr> operation_map);

        std::vector<OperationPtr> GetOperations() const;

        LLIR Lower();
        void Print(std::ofstream& stream) const;

    private:
        std::unordered_map<std::string, VariablePtr> variable_map_;
        std::unordered_map<std::string, OperationPtr> operation_map_;
};

} // namespace core
} // namespace tcc

#endif // TCC_HLIR_H
