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
    next_op_names_ = next_op_names;
}

HLIR::Variable::Variable(
        Data data,
        const std::vector<std::string> next_op_names) {
    CHECK(!data.IsEmpty()) <<
        "data of constant variable can not be empty.";
    CHECK_KEY_NOT_IN_VEC("", next_op_names) <<
        "next_op_name can not be empty.";

    datatype_ = data.GetType();
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
    input_variable_map_ = input_variable_map;
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
    // Check if HLIR is empty
    if (operation_map_.empty()) {
        LOG(ERROR) << "HLIR contains no operaions.";
        return false;
    }

    // Check all variables are connected to existing operations
    // Check all terminal variables are constants or placeholders
    for (std::pair<std::string, Variable> variable_kv : variable_map_) {
        Variable var = variable_kv.second;

        if (var.prev_op_name_.empty()) {
            if (var.datatype_ == Datatype::kUninitialized) {
                LOG(ERROR) << "HLIR terminal variable '" <<
                    variable_kv.first << "' has no datatype / data.";
                return false;
            }
        } else if (operation_map_.find(var.prev_op_name_) == operation_map_.end()) {
            LOG(ERROR) << "HLIR variable '" <<
                variable_kv.first << "' prev_op not found.";
            return false;
        }

        for (std::string next_op_name : var.next_op_names_) {
            if (operation_map_.find(next_op_name) == operation_map_.end()) {
                LOG(ERROR) << "HLIR variable '" <<
                    variable_kv.first << "' next_op not found.";
                return false;
            }
        }
    }

    // Check all operations are connected to existing variables
    for (std::pair<std::string, Operation> op_kv : operation_map_) {
        Operation op = op_kv.second;

        for (std::pair<std::string, std::string> input_kv : op.input_variable_map_) {
            if (variable_map_.find(input_kv.second) == variable_map_.end()) {
                LOG(ERROR) << "Input variable of HLIR operation '" <<
                    op_kv.first << "' is not found.";
                return false;
            }
        }

        for (std::pair<std::string, std::string> output_kv : op.output_variable_map_) {
            if (variable_map_.find(output_kv.second) == variable_map_.end()) {
                LOG(ERROR) << "Output variable of HLIR operation '" <<
                    op_kv.first << "' is not found.";
                return false;
            }
        }
    }

    return true;
}

#define DOT_NODE(node, style) \
    "\t\"" << node << "\" [" << style << "]\n"
#define DOT_EDGE(src, dst, style) \
    "\t\"" << src << "\" -> \"" << dst << "\" [" << style << "]\n"

void HLIR::Print(std::ofstream& stream) const {
    stream << "digraph D {\n";

    for (std::pair<std::string, Variable> var_kv : variable_map_) {
        stream << DOT_NODE(var_kv.first, "shape=box, style=filled, penwidth=0, fillcolor=lightgrey");
        for (std::string next_op_name : var_kv.second.next_op_names_) {

            // Get input name
            std::string input_name;
            CHECK_KEY_IN_MAP(next_op_name, operation_map_) <<
                "HLIR Operation '" << next_op_name << "' is not found.";
            for (std::pair<std::string, std::string> input_kv :
                    operation_map_.at(next_op_name).input_variable_map_) {
                if (input_kv.second == var_kv.first) {
                    input_name = input_kv.first;
                    break;
                }
            }
            CHECK(!input_name.empty()) <<
                "Can not find corresponding input name of '" << var_kv.first << "'.";

            stream << DOT_EDGE(var_kv.first, next_op_name, "label=\" " + input_name + " \"");
        }
    }

    for (std::pair<std::string, Operation> op_kv : operation_map_) {
        stream << DOT_NODE(op_kv.first, "shape=box, style=filled, fillcolor=black, fontcolor=white");
        for (std::pair<std::string, std::string> output_kv : op_kv.second.output_variable_map_) {
            stream << DOT_EDGE(op_kv.first, output_kv.second, "label=\" " + output_kv.first + " \"");
        }
    }

    stream << "}";
}

#undef DOT_NODE
#undef DOT_EDGE

} // namespace core
} // namespace tcc
