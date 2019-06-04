#ifndef TCC_HLIR_H
#define TCC_HLIR_H

#include "tcc/core/common/data.h"
#include "tcc/core/common/operator.h"
#include "tcc/core/hlir/variable.h"
#include "tcc/core/hlir/operation.h"

namespace tcc {
namespace core {
namespace hlir {

class HLIR {
    public:
        void AddVariable(std::string variable_name, common::Datatype datatype);
        void AddVariable(std::string variable_name, common::Datatype datatype, common::Data data);
        void AddOperation(std::string operation_name, common::Operator op);

    private:
        std::unordered_map<std::string, VariablePtr> variable_map_;
        std::unordered_map<std::string, OperationPtr> operation_map_;
};

} // namespace hlir
} // namespace core
} // namespace tcc

#endif // TCC_HLIR_H
