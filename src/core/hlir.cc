#include "tcc/core/hlir.h"

#include <functional>

#include "tcc/core/operator.h"
#include "tcc/core/context.h"

namespace tcc {
namespace core {

HLIR::Variable::Variable(const std::string instance_name, Data data) :
    instance_name_(instance_name),
    data_(data) {
    CHECK(data.Initialized()) <<
        "data_ is uninitialized" JOIN_MSG(for)
        HLIR_VARIABLE_IDENTIFIER(*this);
}

HLIR::Variable::Variable(const std::string instance_name, OperationPtr prev_op) :
    instance_name_(instance_name),
    prev_op_(prev_op) {
    CHECK(prev_op_ != nullptr) <<
        "prev_op_ can not be nullptr" JOIN_MSG(for)
        HLIR_VARIABLE_IDENTIFIER(*this);
}

HLIR::OperationPtr HLIR::Variable::GetPrevOperation() const {
    CHECK(prev_op_ != nullptr) <<
        "Requesting nullptr prev_op_" JOIN_MSG(from)
        HLIR_VARIABLE_IDENTIFIER(*this);
    return prev_op_;
}

std::vector<HLIR::OperationPtr> HLIR::Variable::GetNextOperations() const {
    std::vector<OperationPtr> next_ops;
    for (OperationPtr next_op : next_ops_) {
        CHECK(next_op != nullptr) <<
            "Requesting nullptr next_op" JOIN_MSG(from)
            HLIR_VARIABLE_IDENTIFIER(*this);
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
            KEY_NOT_FOUND_MSG(attr_kv.first, attr_val_map) JOIN_MSG(for)
            HLIR_OPERATION_IDENTIFIER(*this);
        CHECK(attr_kv.second == attr_val_map.at(attr_kv.first).GetType()) <<
            "Attr '" << attr_kv.first << "' datatype mismatch" JOIN_MSG(for)
            HLIR_OPERATION_IDENTIFIER(*this);
    }

    // Check kernel is not nullptr
    CHECK(op.kernel_ != nullptr) <<
        "kernel_ can not be nullptr" JOIN_MSG(for)
        HLIR_OPERATION_IDENTIFIER(*this);

    attr_val_map_ = attr_val_map;
    input_variable_names_ = op.input_names_;
    output_variable_names_ = op.output_names_;
    kernel_ = op.kernel_;
}

Data HLIR::Operation::GetAttr(std::string attr_name) const {
    CHECK_KEY_IN_MAP(attr_name, attr_val_map_) <<
        KEY_NOT_FOUND_MSG(attr_name, attr_val_map_) JOIN_MSG(for)
        HLIR_OPERATION_IDENTIFIER(*this);
    CHECK(attr_val_map_.at(attr_name).Initialized()) <<
        "Attr" QUOTE_MSG(attr_name) "is not initialized" JOIN_MSG(for)
        HLIR_OPERATION_IDENTIFIER(*this);
    return attr_val_map_.at(attr_name);
}

std::string HLIR::Operation::GetInputName(VariablePtr input_variable) const {
    std::string input_name;
    for (std::pair<std::string, WeakVariablePtr> input_kv : input_variable_map_) {
        CHECK_NOT_EXPIRED(input_kv.second);
        if (input_kv.second.lock() == input_variable) {
            CHECK(input_name.empty()) <<
                "Duplicated input variables" JOIN_MSG(for)
                HLIR_OPERATION_IDENTIFIER(*this);
            input_name = input_kv.first;
        }
    }
    CHECK(!input_name.empty()) <<
        "Input not found" JOIN_MSG(for) HLIR_OPERATION_IDENTIFIER(*this);
    return input_name;
}

std::string HLIR::Operation::GetOutputName(VariablePtr output_variable) const {
    std::string output_name;
    for (std::pair<std::string, WeakVariablePtr> output_kv : output_variable_map_) {
        CHECK_NOT_EXPIRED(output_kv.second);
        if (output_kv.second.lock() == output_variable) {
            CHECK(output_name.empty()) <<
                "Duplicated output variables" JOIN_MSG(for)
                HLIR_OPERATION_IDENTIFIER(*this);
            output_name = output_kv.first;
        }
    }
    CHECK(!output_name.empty()) <<
        "Output not found" JOIN_MSG(for) HLIR_OPERATION_IDENTIFIER(*this);
    return output_name;
}

HLIR::VariablePtr HLIR::Operation::GetInputVariable(std::string input_name) const {
    CHECK_KEY_IN_MAP(input_name, input_variable_map_);
    CHECK_NOT_EXPIRED(input_variable_map_.at(input_name));
    return input_variable_map_.at(input_name).lock();
}

HLIR::VariablePtr HLIR::Operation::GetInputVariable(unsigned int input_index) const {
    CHECK_WITHIN_BOUND(input_index, input_variable_names_);
    return GetInputVariable(input_variable_names_.at(input_index));
}

HLIR::VariablePtr HLIR::Operation::GetOutputVariable(std::string output_name) const {
    CHECK_KEY_IN_MAP(output_name, output_variable_map_);
    CHECK_NOT_EXPIRED(output_variable_map_.at(output_name));
    return output_variable_map_.at(output_name).lock();
}

HLIR::VariablePtr HLIR::Operation::GetOutputVariable(unsigned int output_index) const {
    CHECK_WITHIN_BOUND(output_index, output_variable_names_);
    return GetOutputVariable(output_variable_names_.at(output_index));
}

std::vector<HLIR::VariablePtr> HLIR::Operation::GetInputVariables() const {
    std::vector<VariablePtr> input_variables;
    for (std::string input_variable_name : input_variable_names_) {
        CHECK_KEY_IN_MAP(input_variable_name, input_variable_map_) <<
            "Input variable" QUOTE_MSG(input_variable_name) "is not found" JOIN_MSG(for)
            HLIR_OPERATION_IDENTIFIER(*this);
        CHECK_NOT_EXPIRED(input_variable_map_.at(input_variable_name)) <<
            "Input variable" QUOTE_MSG(input_variable_name) "expired" JOIN_MSG(for)
            HLIR_OPERATION_IDENTIFIER(*this);
        input_variables.push_back(
                input_variable_map_.at(input_variable_name).lock());
    }
    return input_variables;
}

std::vector<HLIR::VariablePtr> HLIR::Operation::GetOutputVariables() const {
    std::vector<VariablePtr> output_variables;
    for (std::string output_variable_name : output_variable_names_) {
        CHECK_KEY_IN_MAP(output_variable_name, output_variable_map_) <<
            "Output variable" QUOTE_MSG(output_variable_name) "is not found" JOIN_MSG(for)
            HLIR_OPERATION_IDENTIFIER(*this);
        CHECK_NOT_EXPIRED(output_variable_map_.at(output_variable_name)) <<
            "Output variable" QUOTE_MSG(output_variable_name) "expired" JOIN_MSG(for)
            HLIR_OPERATION_IDENTIFIER(*this);
        output_variables.push_back(
                output_variable_map_.at(output_variable_name).lock());
    }
    return output_variables;
}

std::vector<HLIR::VariablePtr> HLIR::Connect(
        OperationPtr operation,
        std::vector<VariablePtr> input_variables) {
    // Initialize operation with given inputs and produce outputs
    // Check missing input variable
    CHECK(input_variables.size() == operation->input_variable_names_.size()) <<
        "Missing input variables" JOIN_MSG(for) HLIR_OPERATION_IDENTIFIER(*operation);

    // Add this as next_op of input variables
    // Store input variables as weak_ptrs
    CHECK(operation->input_variable_map_.empty()) <<
        "input_variable_map_ already initialized" JOIN_MSG(for)
        HLIR_OPERATION_IDENTIFIER(*operation);
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
    std::string(*GenerateUniqueVariableName)() = []() -> std::string {
        static unsigned long counter = 0; counter++;
        return "V" + std::to_string(counter);
    };

    CHECK(operation->output_variable_map_.empty()) <<
        "output_variable_map_ already initialized" JOIN_MSG(for)
        HLIR_OPERATION_IDENTIFIER(*operation);
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
                KEY_NOT_FOUND_MSG(prev_op->instance_name_, operation_map);
            CHECK(operation_map.at(prev_op->instance_name_) == prev_op) <<
                "Pointer mismatch" JOIN_MSG(for) HLIR_OPERATION_IDENTIFIER(*prev_op);
        }

        for (OperationPtr next_op : variable_kv.second->GetNextOperations()) {
            CHECK_KEY_IN_MAP(next_op->instance_name_, operation_map) <<
                KEY_NOT_FOUND_MSG(next_op->instance_name_, operation_map);
            CHECK(operation_map.at(next_op->instance_name_) == next_op) <<
                "Pointer mismatch" JOIN_MSG(for) HLIR_OPERATION_IDENTIFIER(*next_op);
        }
    }

    // Check all operation inputs and outputs exists in variable_map
    // and have unexpired matching pointer addresses
    for (std::pair<std::string, OperationPtr> operation_kv : operation_map) {

        for (VariablePtr input_variable : operation_kv.second->GetInputVariables()) {
            CHECK_KEY_IN_MAP(input_variable->instance_name_, variable_map) <<
                KEY_NOT_FOUND_MSG(input_variable->instance_name_, variable_map);
            CHECK(variable_map.at(input_variable->instance_name_) == input_variable) <<
                "Pointer mismatch" JOIN_MSG(for) HLIR_VARIABLE_IDENTIFIER(*input_variable);
        }

        for (VariablePtr output_variable : operation_kv.second->GetOutputVariables()) {
            CHECK_KEY_IN_MAP(output_variable->instance_name_, variable_map) <<
                KEY_NOT_FOUND_MSG(output_variable->instance_name_, variable_map);
            CHECK(variable_map.at(output_variable->instance_name_) == output_variable) <<
                "Pointer mismatch" JOIN_MSG(for) HLIR_VARIABLE_IDENTIFIER(*output_variable);
        }
    }

    variable_map_ = variable_map;
    operation_map_ = operation_map;
}

std::vector<HLIR::OperationPtr> HLIR::GetOperations() const {
    // Sort HLIR operations by data dependency
    std::vector<OperationPtr> operations;
    std::unordered_set<OperationPtr> traversed;

    std::function<void(OperationPtr)> Recurse;
    Recurse = [&operations, &traversed, &Recurse](OperationPtr operation) {
        traversed.insert(operation);
        for (VariablePtr input_variable : operation->GetInputVariables()) {
            if (!input_variable->Terminal()) {
                // Recurse further if input Variable is an intermediate
                Recurse(input_variable->GetPrevOperation());
            }
        }
        operations.push_back(operation);
    };

    for (std::pair<std::string, OperationPtr> operation_kv : operation_map_) {
        if (traversed.find(operation_kv.second) == traversed.end()) {
            Recurse(operation_kv.second);
        }
    }

    return operations;
}

LLIR HLIR::Lower() {
    // Convert HLIR into LLIR
    OperationPtr operation = GetOperations()[0];
    //for (OperationPtr operation : GetOperations()) {
        KernelContext ctx(operation);
        operation->kernel_(ctx);
    //}

    return LLIR();
}

void HLIR::Print(std::ofstream& stream) const {
    stream << "digraph D {\n";

#define DOT_NODE(node, style) \
    "\t\"" << node << "\" [" << style << "]\n"
#define DOT_EDGE(src, dst, label) \
    "\t\"" << src << "\" -> \"" << dst << "\" [label=\" " << label << " \"]\n"

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

#undef DOT_NODE
#undef DOT_EDGE

    stream << "}";
}

} // namespace core
} // namespace tcc
