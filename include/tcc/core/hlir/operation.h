#ifndef TCC_HLIR_OPERATION_H
#define TCC_HLIR_OPERATION_H

#include <memory>

#include "tcc/core/common/operator.h"
#include "tcc/core/hlir/variable.h"

namespace tcc {
namespace core {
namespace hlir {

class Operation {
    public:
        Operation(const std::string instance_name, const common::Operator& op);

    private:
        const std::string instance_name_;
        const std::unordered_map<std::string, common::DataPtr> attrs_;
        const std::vector<std::string> input_list_;
        const std::vector<std::string> output_list_;
        std::unordered_map<std::string, WeakVariablePtr> input_variable_map_;
        std::unordered_map<std::string, WeakVariablePtr> output_variable_map_;
};

typedef std::shared_ptr<Operation> OperationPtr;

} // namespace hlir
} // namespace core
} // namespace tcc

#endif // TCC_HLIR_OPERATION_H
