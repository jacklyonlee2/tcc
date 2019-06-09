#include "tcc/core/hlir.h"

namespace tcc {
namespace core {

HLIR::Variable::Variable(
        const std::string prev_op_name,
        const std::vector<std::string> next_op_names) {
    CHECK(!prev_op_name.empty()) <<
        "prev_op_name of intermidate variable can not be empty.";
    CHECK_KEY_NOT_IN_VEC("", next_op_names) <<
        "next_op_name can not be empty.";

    prev_op_name_ = prev_op_name;
    next_op_names_ = next_op_names_;
}

HLIR::Variable::Variable(
        Data data,
        const std::vector<std::string> next_op_names) {
    CHECK(!data.IsEmpty()) <<
        "data of constant variable can not be empty.";
    CHECK_KEY_NOT_IN_VEC("", next_op_names) <<
        "next_op_name can not be empty.";

    datatype_ = data_.GetType();
    data_ = data;
    next_op_names_ = next_op_names;
}

HLIR::Variable::Variable(
        Datatype datatype,
        const std::vector<std::string> next_op_names) {
    CHECK(datatype != Datatype::kUninitialized) <<
        "datatype of placeholder variable can not be 'kUninitialized'.";
    CHECK_KEY_NOT_IN_VEC("", next_op_names) <<
        "next_op_name can not be empty.";

    datatype_ = datatype;
    next_op_names_ = next_op_names;
}

HLIR::Operation::Operation(
        const std::unordered_map<std::string, Data> attrs,
        const std::unordered_map<std::string, std::string> input_variable_map,
        const std::unordered_map<std::string, std::string> output_variable_map) {
    for (std::pair<std::string, Data> attr_kv : attrs) {
        CHECK(!attr_kv.second.IsEmpty()) << "Attr value can not be empty.";
    }

    for (std::pair<std::string, std::string> input_kv : input_variable_map) {
        CHECK(!input_kv.second.empty()) << "input_variable_name can not be empty.";
    }

    for (std::pair<std::string, std::string> output_kv : output_variable_map) {
        CHECK(!output_kv.second.empty()) << "output_variable_name can not be empty.";
    }

    attrs_ = attrs;
    input_variable_map_ = input_variable_map_;
    output_variable_map_ = output_variable_map;
}

void HLIR::AddVariable(std::string name, Variable variable) {
    CHECK_KEY_NOT_IN_MAP(name, variable_map_);
    CHECK_KEY_NOT_IN_MAP(name, operation_map_);
    CHECK(!name.empty()) << "Variable name can not be empty.";

    variable_map_.insert({name, variable});
}

void HLIR::AddOperation(std::string name, Operation operation) {
    CHECK_KEY_NOT_IN_MAP(name, variable_map_);
    CHECK_KEY_NOT_IN_MAP(name, operation_map_);
    CHECK(!name.empty()) << "Operation name can not be empty.";

    operation_map_.insert({name, operation});
}

bool HLIR::IsValid() const {
    // Check all variables are connected to existing operations
    // Check all terminal variables are constants or placeholders
    for (std::pair<std::string, Variable> variable_kv : variable_map_) {
        Variable var = variable_kv.second;

        if (var.prev_op_name_.empty()) {
            if (var.datatype_ == Datatype::kUninitialized) {
                LOG(ERROR) << "HLIR terminal variable has no datatype / data.";
                return false;
            }
        } else if (operation_map_.find(var.prev_op_name_) == operation_map_.end()) {
            LOG(ERROR) << "HLIR variable prev_op not found.";
            return false;
        }

        for (std::string next_op_name : var.next_op_names_) {
            if (operation_map_.find(next_op_name) == operation_map_.end()) {
                LOG(ERROR) << "HLIR variable next_op not found.";
                return false;
            }
        }
    }

    // Check all operations are connected to existing variables
    for (std::pair<std::string, Operation> operation_kv : operation_map_) {
        Operation op = operation_kv.second;

        for (std::pair<std::string, std::string> input_kv : op.input_variable_map_) {
            if (variable_map_.find(input_kv.second) == variable_map_.end()) {
                LOG(ERROR) << "HLIR operation input variable not found.";
                return false;
            }
        }

        for (std::pair<std::string, std::string> output_kv : op.output_variable_map_) {
            if (variable_map_.find(output_kv.second) == variable_map_.end()) {
                LOG(ERROR) << "HLIR operation output variable not found.";
                return false;
            }
        }
    }

    return true;
}

} // namespace core
} // namespace tcc
