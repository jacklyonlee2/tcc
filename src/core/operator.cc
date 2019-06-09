#include "tcc/core/operator.h"

#include "tcc/core/operator_registry.h"

namespace tcc {
namespace core {

OperatorBuilder::OperatorBuilder(const std::string type_name) :
    type_name_(type_name) {
}

OperatorBuilder& OperatorBuilder::Attr(const std::string attr_name, const Datatype datatype) {
    // This function is called at static initialization time, NATIVE_CHECK is used instead of CHECK.
    NATIVE_CHECK_KEY_NOT_IN_MAP(attr_name, attr_type_map_,
            "'" + attr_name + "' already exists in attr_type_map_.");
    NATIVE_CHECK(datatype != Datatype::kUninitialized,
            "Attr data type can not be 'kUninitialized'.");
    attr_type_map_.insert({attr_name, datatype});
    return *this;
}

OperatorBuilder& OperatorBuilder::Input(const std::string input_name) {
    // This function is called at static initialization time, NATIVE_CHECK is used instead of CHECK.
    NATIVE_CHECK_KEY_NOT_IN_VEC(input_name, input_list_,
            "'" + input_name + "' already exists in input_list_.");
    input_list_.push_back(input_name);
    return *this;
}

OperatorBuilder& OperatorBuilder::Output(const std::string output_name) {
    // This function is called at static initialization time, NATIVE_CHECK is used instead of CHECK.
    NATIVE_CHECK_KEY_NOT_IN_VEC(output_name, output_list_,
            "'" + output_name + "' already exists in output_list_.");
    output_list_.push_back(output_name);
    return *this;
}

Operator::Operator(OperatorBuilder builder) :
    attr_type_map_(builder.attr_type_map_),
    input_list_(builder.input_list_),
    output_list_(builder.output_list_) {
    // Initialize attr_val_map_ with empty data
    for (std::pair<std::string, Datatype> attr_type_kv : builder.attr_type_map_) {
        attr_val_map_.insert({attr_type_kv.first, Data()});
    }

    // Store operator into static operator registry
    OperatorRegistry::Register(builder.type_name_, *this);
}

void Operator::SetAttr(std::string attr_name, Data attr_val) {
    CHECK_KEY_IN_MAP(attr_name, attr_type_map_) <<
        "Attr '" << attr_name << "' is not found in attr_type_map_.";
    CHECK_KEY_IN_MAP(attr_name, attr_val_map_) <<
        "Attr '" << attr_name << "' is not found in attr_val_map_.";
    CHECK(attr_val_map_.at(attr_name).IsEmpty()) <<
        "Attr data for '" << attr_name << "' is already set";
    CHECK(attr_type_map_.at(attr_name) == attr_val.GetType()) <<
        "Actual attr type does not agree with declared attr type.";

    attr_val_map_.at(attr_name) = attr_val;
}

} // namespace core
} // namespace tcc
