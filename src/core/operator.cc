#include "tcc/core/operator.h"

#include "tcc/core/operator_builder.h"
#include "glog/logging.h"

namespace tcc {
namespace core {

Operator::Operator(OperatorBuilder& builder) {
    LOG(INFO) << "Recieved op " << builder.type_name_;
}

} // namespace core
} // namespace tcc
