#include "tcc/core/operator.h"

#include "tcc/core/operator_registry.h"

namespace tcc {
namespace core {

OperatorBuilder::OperatorBuilder(const std::string type_name) :
    type_name_(type_name) {
}

OperatorBuilder& OperatorBuilder::Attr(const std::string attr_name, const Datatype datatype) {
    // This function is called at static initialization time, NATIVE_CHECK is used instead of CHECK.
    NATIVE_CHECK_KEY_NOT_IN_MAP(attr_name, attr_type_map_);
    attr_type_map_.insert({attr_name, datatype});
    return *this;
}

OperatorBuilder& OperatorBuilder::Input(const std::string input_name) {
    // This function is called at static initialization time, NATIVE_CHECK is used instead of CHECK.
    NATIVE_CHECK_KEY_NOT_IN_VEC(input_name, input_list_);
    input_list_.push_back(input_name);
    return *this;
}

OperatorBuilder& OperatorBuilder::Output(const std::string output_name) {
    // This function is called at static initialization time, NATIVE_CHECK is used instead of CHECK.
    NATIVE_CHECK_KEY_NOT_IN_VEC(output_name, output_list_);
    output_list_.push_back(output_name);
    return *this;
}

Operator::Operator(OperatorBuilder builder) :
    type_name_(builder.type_name_),
    attr_type_map_(builder.attr_type_map_),
    input_list_(builder.input_list_),
    output_list_(builder.output_list_) {
    // Store operator into static operator registry
    OperatorRegistry::Register(builder.type_name_, *this);
}

void Operator::SetAttr(std::string attr_name, Data attr_val) {
    CHECK_KEY_IN_MAP(attr_name, attr_type_map_);
    CHECK_KEY_NOT_IN_MAP(attr_name, attr_val_map_);
    CHECK(attr_type_map_.at(attr_name) == attr_val.GetType()) <<
        "Actual attr type does not agree with declared attr type.";

    attr_val_map_.insert({attr_name, attr_val});
}

} // namespace core
} // namespace tcc
