#include "tcc/core/hlir.h"

#include <functional>

#include "tcc/core/operator.h"

namespace tcc {
namespace core {

HLIR::Variable::Variable(const std::string instance_name, Data data) :
    instance_name_(instance_name),
    data_(data) {
    CHECK(data.Initialized()) <<
        "data_ for constant HLIR::Variable can not be uninitialized.";
}

HLIR::Variable::Variable(const std::string instance_name, OperationPtr prev_op) :
    instance_name_(instance_name),
    prev_op_(prev_op) {
    CHECK(prev_op_ != nullptr) <<
        "prev_op_ for intermediate HLIR::Variable can not be nullptr.";
}

HLIR::OperationPtr HLIR::Variable::GetPrevOperation() const {
    CHECK(prev_op_ != nullptr) <<
        "prev_op_ for HLIR::Variable '" << instance_name_ << "' is nullptr.";
    return prev_op_;
}

std::vector<HLIR::OperationPtr> HLIR::Variable::GetNextOperations() const {
    std::vector<OperationPtr> next_ops;
    for (OperationPtr next_op : next_ops_) {
        CHECK(next_op != nullptr) <<
            "next_op for HLIR::Variable '" << instance_name_ << "' is nullptr.";
        next_ops.push_back(next_op);
    }
    return next_ops;
}

HLIR::Operation::Operation(
        const std::string instance_name,
        const std::string type_name,
        std::unordered_map<std::string, Data> attr_val_map) :
    instance_name_(instance_name),
    type_name_(type_name) {
    // Create Operator of type_name
    Operator op = Operator::Create(type_name);

    // Check missing attr and attr datatype mismatch
    for (std::pair<std::string, Datatype> attr_kv : op.attr_type_map_) {
        CHECK_KEY_IN_MAP(attr_kv.first, attr_val_map) <<
            "Missing attr '" << attr_kv.first << "' for operation '" <<
            instance_name_ << "' of type '" << type_name_ << "'.";
        CHECK(attr_kv.second == attr_val_map.at(attr_kv.first).GetType()) <<
            "Mismatched datatype for attr '" << attr_kv.first <<
            "' for operation '" << instance_name_ << "' of type '" <<
            type_name_ << "'.";
    }

    attr_val_map_ = attr_val_map;
    input_variable_names_ = op.input_names_;
    output_variable_names_ = op.output_names_;
}

std::string HLIR::Operation::GetInputName(VariablePtr input_variable) const {
    for (std::pair<std::string, WeakVariablePtr> input_kv : input_variable_map_) {
        CHECK_NOT_EXPIRED(input_kv.second);
        if (input_kv.second.lock() == input_variable) {
            return input_kv.first;
        }
    }
    LOG(FATAL) << "input_variable '" << input_variable->instance_name_ <<
        "' is not found in input_variable_map_.";
}

std::string HLIR::Operation::GetOutputName(VariablePtr output_variable) const {
    for (std::pair<std::string, WeakVariablePtr> output_kv : output_variable_map_) {
        CHECK_NOT_EXPIRED(output_kv.second);
        if (output_kv.second.lock() == output_variable) {
            return output_kv.first;
        }
    }
    LOG(FATAL) << "output_variable '" << output_variable->instance_name_ <<
        "' is not found in output_variable_map_.";
}

HLIR::VariablePtr HLIR::Operation::GetOutputVariable(unsigned int index) const {
    CHECK_WITHIN_BOUND(index, output_variable_names_);
    CHECK_KEY_IN_MAP(output_variable_names_.at(index), output_variable_map_);
    CHECK_NOT_EXPIRED(output_variable_map_.at(output_variable_names_.at(index)));
    return output_variable_map_.at(output_variable_names_.at(index)).lock();
}

std::vector<HLIR::VariablePtr> HLIR::Operation::GetInputVariables() const {
    std::vector<VariablePtr> input_variables;
    for (std::string input_variable_name : input_variable_names_) {
        CHECK_KEY_IN_MAP(input_variable_name, input_variable_map_) <<
            "Input variable '" << input_variable_name << "' is not found.";
        CHECK_NOT_EXPIRED(input_variable_map_.at(input_variable_name)) <<
            "weak_ptr for input variable '" << input_variable_name << "' expired.";
        input_variables.push_back(
                input_variable_map_.at(input_variable_name).lock());
    }
    return input_variables;
}

std::vector<HLIR::VariablePtr> HLIR::Operation::GetOutputVariables() const {
    std::vector<VariablePtr> output_variables;
    for (std::string output_variable_name : output_variable_names_) {
        CHECK_KEY_IN_MAP(output_variable_name, output_variable_map_) <<
            "Output variable '" << output_variable_name << "' is not found.";
        CHECK_NOT_EXPIRED(output_variable_map_.at(output_variable_name)) <<
            "weak_ptr for output variable '" << output_variable_name << "' expired.";
        output_variables.push_back(
                output_variable_map_.at(output_variable_name).lock());
    }
    return output_variables;
}

static std::string GenerateUniqueVariableName() {
    static unsigned long counter = 0; counter++;
    return "V" + std::to_string(counter);
}

std::vector<HLIR::VariablePtr> HLIR::Operation::Infer(
        OperationPtr operation,
        std::vector<VariablePtr> input_variables) {
    // Check missing input variable
    CHECK(input_variables.size() == operation->input_variable_names_.size()) <<
        "Missing input variables for operation '" << operation->instance_name_ <<
         "' of type '" << operation->type_name_ << "'; given " <<
         input_variables.size() << " but requires " <<
         operation->input_variable_names_.size();

    // Add this as next_op of input variables
    // Store input variables as weak_ptrs
    CHECK(operation->input_variable_map_.empty()) <<
        "input_variable_map_ is already initialized for operation '" <<
        operation->instance_name_ << "'.";
    for (unsigned int input_index = 0;
            input_index < operation->input_variable_names_.size();
            input_index++) {

        input_variables.at(input_index)->next_ops_.insert(
                operation);
        operation->input_variable_map_.insert({
                operation->input_variable_names_.at(input_index),
                input_variables.at(input_index)});
    }

    // Create and store output variables
    // Set this as prev_op of output variables
    CHECK(operation->output_variable_map_.empty()) <<
        "output_variable_map_ is already initialized for operation '" <<
        operation->instance_name_ << "'.";
    std::vector<VariablePtr> output_variables;
    for (unsigned int output_index = 0;
            output_index < operation->output_variable_names_.size();
            output_index++) {

        output_variables.push_back(
                std::make_shared<Variable>(
                    GenerateUniqueVariableName(),
                    operation));
        operation->output_variable_map_.insert({
                operation->output_variable_names_.at(output_index),
                output_variables.at(output_index)});
    }

    return output_variables;
}

HLIR::HLIR(std::unordered_map<std::string, VariablePtr> variable_map,
        std::unordered_map<std::string, OperationPtr> operation_map) {
    // Check all variable prev and next ops exists in operation_map
    // Check terminal variable contains Datatype or Data
    for (std::pair<std::string, VariablePtr> variable_kv : variable_map) {

        if (!variable_kv.second->data_.Initialized()) {
            OperationPtr prev_op = variable_kv.second->GetPrevOperation();
            CHECK_KEY_IN_MAP(prev_op->instance_name_, operation_map) <<
                "HLIR::Operation '" << prev_op->instance_name_ << "' is not found.";
            CHECK(operation_map.at(prev_op->instance_name_) == prev_op) <<
                "Pointer for HLIR::Operation '" << prev_op->instance_name_ <<
                "' does not match.";
        }

        for (OperationPtr next_op : variable_kv.second->GetNextOperations()) {
            CHECK_KEY_IN_MAP(next_op->instance_name_, operation_map) <<
                "HLIR::Operation '" << next_op->instance_name_ << "' is not found.";
            CHECK(operation_map.at(next_op->instance_name_) == next_op) <<
                "Pointer for HLIR::Operation '" << next_op->instance_name_ <<
                "' does not match.";
        }
    }

    // Check all operation inputs and outputs exists in variable_map
    // and have unexpired matching pointer addresses
    for (std::pair<std::string, OperationPtr> operation_kv : operation_map) {

        for (VariablePtr input_variable : operation_kv.second->GetInputVariables()) {
            CHECK_KEY_IN_MAP(input_variable->instance_name_, variable_map) <<
                "HLIR::Variable '" << input_variable->instance_name_ << "' is not found.";
            CHECK(variable_map.at(input_variable->instance_name_) == input_variable) <<
                "Pointer for HLIR::Variable '" << input_variable->instance_name_ <<
                "' does not match.";
        }

        for (VariablePtr output_variable : operation_kv.second->GetOutputVariables()) {
             CHECK_KEY_IN_MAP(output_variable->instance_name_, variable_map) <<
                 "HLIR::Variable '" << output_variable->instance_name_ << "' is not found.";
             CHECK(variable_map.at(output_variable->instance_name_) == output_variable) <<
                 "Pointer for HLIR::Variable '" << output_variable->instance_name_ <<
                 "' does not match.";
        }
    }

    variable_map_ = variable_map;
    operation_map_ = operation_map;
}

#define DOT_NODE(node, style) \
    "\t\"" << node << "\" [" << style << "]\n"
#define DOT_EDGE(src, dst, label) \
    "\t\"" << src << "\" -> \"" << dst << "\" [label=\" " << label << " \"]\n"

void HLIR::Print(std::ofstream& stream) const {
    stream << "digraph D {\n";

    for (std::pair<std::string, VariablePtr> variable_kv : variable_map_) {
        stream << DOT_NODE(variable_kv.first,
                "shape=box, style=filled, penwidth=0, fillcolor=lightgrey");

        for (OperationPtr next_op : variable_kv.second->GetNextOperations()) {
            stream << DOT_EDGE(variable_kv.first, next_op->instance_name_,
                    next_op->GetInputName(variable_kv.second));
        }
    }

    for (std::pair<std::string, OperationPtr> operation_kv : operation_map_) {
        stream << DOT_NODE(operation_kv.first,
                "shape=box, style=filled, fillcolor=black, fontcolor=white");

        for (VariablePtr output_variable : operation_kv.second->GetOutputVariables()) {
            stream << DOT_EDGE(operation_kv.first, output_variable->instance_name_,
                    operation_kv.second->GetOutputName(output_variable));
        }
    }

    stream << "}";
}

#undef DOT_NODE
#undef DOT_EDGE

} // namespace core
} // namespace tcc
