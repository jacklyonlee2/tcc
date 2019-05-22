#include "tcc/core/operator_builder.h"

#include <string>

#include "glog/logging.h"

namespace tcc {
namespace core {

OperatorBuilder::OperatorBuilder(std::string type_name) :
    type_name_(type_name) {
    LOG(INFO) << "Constructing builder of " << type_name;
}

OperatorBuilder& OperatorBuilder::Input(std::string input_name) {
    LOG(INFO) << "Input " << input_name;
    return *this;
}

OperatorBuilder& OperatorBuilder::Output(std::string output_name) {
    LOG(INFO) << "Output " << output_name;
    return *this;
}

} // namespace core
} // namespace tcc
