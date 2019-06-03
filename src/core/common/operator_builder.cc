#include "tcc/core/common/operator_builder.h"

#include <string>

#include "tcc/util/logging.h"

namespace tcc {
namespace core {
namespace common {

OperatorBuilder::OperatorBuilder(const std::string type_name) :
    type_name_(type_name) {
}

OperatorBuilder& OperatorBuilder::Attr(const std::string attr_name, const Datatype datatype) {
    // This function is called at static initialization time, NATIVE_CHECK is used instead of CHECK.
    NATIVE_CHECK_KEY_NOT_IN_MAP(attr_name, attr_type_map_);
    attr_type_map_.insert({attr_name, datatype});
    return *this;
}

OperatorBuilder& OperatorBuilder::Input(const std::string input_name, const Datatype datatype) {
    // This function is called at static initialization time, NATIVE_CHECK is used instead of CHECK.
    NATIVE_CHECK_KEY_NOT_IN_MAP(input_name, input_type_map_);
    input_type_map_.insert({input_name, datatype});
    return *this;
}

OperatorBuilder& OperatorBuilder::Output(const std::string output_name, const Datatype datatype) {
    // This function is called at static initialization time, NATIVE_CHECK is used instead of CHECK.
    NATIVE_CHECK_KEY_NOT_IN_MAP(output_name, output_type_map_);
    output_type_map_.insert({output_name, datatype});
    return *this;
}

} // namespace common
} // namespace core
} // namespace tcc
