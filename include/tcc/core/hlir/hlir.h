#ifndef TCC_HLIR_H
#define TCC_HLIR_H

#include "tcc/core/common/data.h"
#include "tcc/core/common/operator.h"

namespace tcc {
namespace core {
namespace hlir {

class HLIR {
    public:
        void AddVariable(std::string variable_name, common::Datatype datatype);
        void AddVariable(std::string variable_name, common::Datatype datatype, common::Data data);
        void AddOperation(std::string operation_name, common::Operator op);
};

} // namespace hlir
} // namespace core
} // namespace tcc

#endif // TCC_HLIR_H
