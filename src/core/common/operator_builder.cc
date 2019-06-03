#include "tcc/core/common/operator_builder.h"

#include <string>

#include "tcc/util/logging.h"

namespace tcc {
namespace core {
namespace common {

OperatorBuilder::OperatorBuilder(const std::string type_name) :
    type_name_(type_name) {
}

OperatorBuilder& OperatorBuilder::Attr(const std::string attr_name) {
    return *this;
}

OperatorBuilder& OperatorBuilder::Input(const std::string input_name) {
    return *this;
}

OperatorBuilder& OperatorBuilder::Output(const std::string output_name) {
    return *this;
}

} // namespace common
} // namespace core
} // namespace tcc
