#include "tcc/core/operator.h"

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

// Initialize static variable Operator::registry_
std::shared_ptr<std::unordered_map<std::string, Operator>> Operator::registry_;

Operator::Operator(OperatorBuilder builder) :
    type_name_(builder.type_name_),
    attr_type_map_(builder.attr_type_map_),
    input_list_(builder.input_list_),
    output_list_(builder.output_list_) {
    // This function is called at static initialization time, NATIVE_CHECK is used instead of CHECK.
    // Because static initalization order is random, Register could be called before registry_ is initalized.
    // The following lines ensure registry_ is initialize before any op registration.
    if (registry_ == nullptr) {
        registry_ = std::shared_ptr<std::unordered_map<std::string, Operator>>(
                new std::unordered_map<std::string, Operator>());
    }

    NATIVE_CHECK_KEY_NOT_IN_MAP(type_name_, *registry_,
        "Operator type '" + type_name_ + "' has already been registered.");
    NATIVE_CHECK(!type_name_.empty(),
        "Operator type name can not be empty.");

    registry_->insert({type_name_, *this});
}

bool Operator::Exists(std::string type_name) {
    CHECK(registry_ != nullptr) <<
        "Operator registry is not initialized.";

    return registry_->find(type_name) != registry_->end();
}

Operator Operator::Create(std::string type_name) {
    CHECK(registry_ != nullptr) <<
        "Operator registry is not initialized.";
    CHECK_KEY_IN_MAP(type_name, *registry_) <<
        "Operator type '" << type_name << "' is not registered.";

    return registry_->at(type_name);
}

} // namespace core
} // namespace tcc
