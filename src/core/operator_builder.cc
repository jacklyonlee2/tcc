#include "tcc/core/operator_builder.h"

#include <string>

#include "tcc/util/logging.h"

namespace tcc {
namespace core {

OperatorBuilder::OperatorBuilder(std::string type_name) :
    type_name_(type_name) {
}

OperatorBuilder& OperatorBuilder::Attr(std::string attr_name) {
    return *this;
}

OperatorBuilder& OperatorBuilder::Input(std::string input_name) {
    return *this;
}

OperatorBuilder& OperatorBuilder::Output(std::string output_name) {
    return *this;
}

} // namespace core
} // namespace tcc
