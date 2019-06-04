#ifndef TCC_HLIR_VARIABLE_H
#define TCC_HLIR_VARIABLE_H

#include <memory>

#include "tcc/core/common/data.h"

namespace tcc {
namespace core {
namespace hlir {

class Variable {
    public:
        Variable(std::string instance_name, common::Datatype datatype);
        Variable(std::string instance_name, common::DataPtr data);

    private:
        const std::string instance_name_;
        common::Datatype datatype_;
        common::DataPtr data_ = nullptr;
};

typedef std::shared_ptr<Variable> VariablePtr;

} // namespace hlir
} // namespace core
} // namespace tcc

#endif // TCC_HLIR_VARIABLE_H
