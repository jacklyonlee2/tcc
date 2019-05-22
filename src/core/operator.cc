#include "tcc/core/operator.h"

#include "tcc/core/operator_registry.h"
#include "tcc/core/operator_builder.h"
#include "glog/logging.h"

namespace tcc {
namespace core {

Operator::Operator(OperatorBuilder& builder) {
    LOG(INFO) << "Recieved op " << builder.type_name_;
    OperatorRegistry::Register(builder.type_name_, *this);
}

} // namespace core
} // namespace tcc
