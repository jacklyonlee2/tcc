#ifndef TCC_HLIR_OPERATION_H
#define TCC_HLIR_OPERATION_H

#include <memory>

#include "tcc/core/common/operator.h"

namespace tcc {
namespace core {
namespace hlir {

class Operation {
    public:
        Operation(std::string instance_name, common::Operator op);

    private:
        const std::string instance_name_;
        const std::unordered_map<std::string, common::DataPtr> attrs_;
};

typedef std::shared_ptr<Operation> OperationPtr;

} // namespace hlir
} // namespace core
} // namespace tcc

#endif // TCC_HLIR_OPERATION_H
