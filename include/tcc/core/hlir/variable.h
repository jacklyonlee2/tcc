#ifndef TCC_HLIR_VARIABLE_H
#define TCC_HLIR_VARIABLE_H

#include <memory>

#include "tcc/core/common/data.h"

namespace tcc {
namespace core {
namespace hlir {

class Operation;
typedef std::shared_ptr<Operation> OperationPtr;

class Variable {
    public:
        Variable(const std::string instance_name, const common::Datatype datatype);
        Variable(const std::string instance_name, common::Data data);

    private:
        const std::string instance_name_;
        const common::Datatype datatype_;
        common::DataPtr data_ = nullptr;

        OperationPtr prev_operation = nullptr;
        std::vector<OperationPtr> next_operations;
};

typedef std::shared_ptr<Variable> VariablePtr;
typedef std::weak_ptr<Variable> WeakVariablePtr;

} // namespace hlir
} // namespace core
} // namespace tcc

#endif // TCC_HLIR_VARIABLE_H
