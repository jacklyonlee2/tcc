#include "tcc/core/context.h"

#include "tcc/core/logging.h"

namespace tcc {
namespace core {

ParserContext::ParserContext(const std::string input_path) :
    input_path_(input_path) {}

bool ParserContext::OperatorExists(const std::string type) {
    CHECK_NOTNULL(OperatorRegistry::registry_);

    return OperatorRegistry::registry_->find(type) !=
        OperatorRegistry::registry_->end();
}

Operator ParserContext::OperatorInstantiate(const std::string type) {
    CHECK_NOTNULL(OperatorRegistry::registry_);
    CHECK_KEY_IN_MAP(type, *OperatorRegistry::registry_) <<
        "Operator type '" << type << "' is not found in registry_.";

    return OperatorRegistry::registry_->at(type);
}

void ParserContext::AddPlaceholder(const std::string name, Datatype datatype) {
    CHECK(!name.empty()) <<
        "Placeholder name can not be empty.";
    CHECK(datatype != Datatype::kUninitialized) <<
        "Placeholder data type can not be 'kUninitialized'.";
    CHECK_KEY_NOT_IN_MAP(name, placeholder_map_) <<
        "Placeholder '" << name << "' already exists.";

    placeholder_map_.insert({name, datatype});
}

void ParserContext::AddConstant(const std::string name, Data data) {
    CHECK(!name.empty()) <<
        "Constant name can not be empty.";
    CHECK(!data.IsEmpty()) <<
        "Constant data can not be uninitialized.";
    CHECK_KEY_NOT_IN_MAP(name, constant_map_) <<
        "Constant '" << name << "' already exists.";

    constant_map_.insert({name, data});
}

void ParserContext::AddOperation(const std::string name, Operator op) {
    CHECK(!name.empty()) <<
        "Operation name can not be empty.";
    CHECK_KEY_NOT_IN_MAP(name, operation_map_) <<
        "Operation '" << name << "' already exists.";

    operation_map_.insert({name, op});
}

void ParserContext::AddEdge(const std::string src_name, const unsigned int output_index,
        const std::string dst_name,  const unsigned int input_index) {
    CHECK(!src_name.empty()) <<
        "src_name can not be empty.";
    CHECK(!dst_name.empty()) <<
        "dst_name can not be empty.";

    edges_.push_back({{src_name, output_index}, {dst_name, input_index}});
}

HLIR ParserContext::BuildHLIR() {
    unsigned int intermidiate_count = 1;
    std::unordered_map<std::string, std::string> intermidiate_prev_op_map;
    std::unordered_map<std::string, std::vector<std::string>> var_next_ops_map;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> op_input_map;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> op_output_map;

    for (Edge edge : edges_) {
        const std::string src_name = edge.first.first;
        const unsigned int output_index = edge.first.second;
        const std::string dst_name = edge.second.first;
        const unsigned int input_index = edge.second.second;

        if (placeholder_map_.find(src_name) != placeholder_map_.end() ||
                constant_map_.find(src_name) != constant_map_.end()) {
            // Connect src constant or placeholder var to dst operation
            CHECK(output_index == 0) <<
                "output_index must be 0 for constants or placeholders.";
            CHECK_KEY_IN_MAP(dst_name, operation_map_) <<
                "'" << dst_name << "' is not found as an operation.";
            CHECK(input_index < operation_map_.at(dst_name).input_list_.size()) <<
                "input_index '" << input_index << "' for operator '" <<
                operation_map_.at(dst_name).type_name_ << "' is out of bound.";

            const std::string op_input_name =
                operation_map_.at(dst_name).input_list_[input_index];

            // Add dst_name as a next_op to src constant or placeholder
            if (var_next_ops_map.find(src_name) != var_next_ops_map.end()) {
                CHECK_KEY_NOT_IN_VEC(dst_name, var_next_ops_map.at(src_name)) <<
                    "'" << dst_name << "' already exists in next_ops of '" << src_name << "'.";
                var_next_ops_map.at(src_name).push_back(dst_name);
            } else {
                var_next_ops_map.insert({src_name, {dst_name}});
            }

            // Add src constant or placeholder as an input to dst operation
            if (op_input_map.find(dst_name) != op_input_map.end()) {

                CHECK_KEY_NOT_IN_MAP(op_input_name, op_input_map.at(dst_name)) <<
                    "Input '" << op_input_name << "' for '" << dst_name << "' is already set.";
                op_input_map.at(dst_name).insert({op_input_name, src_name});
            } else {
                op_input_map.insert({dst_name, {{op_input_name, src_name}}});
            }

        } else {
            // Initialize intermidiate var
            std::string intermidiate_name;

            // Connect src operation to intermidiate var
            CHECK_KEY_IN_MAP(src_name, operation_map_) <<
                "'" << src_name << "' is not found as an operation.";
            CHECK(output_index < operation_map_.at(src_name).output_list_.size()) <<
                "output_index '" << output_index << "' for operator '" <<
                operation_map_.at(src_name).type_name_ << "' is out of bound.";

            const std::string op_output_name =
                operation_map_.at(src_name).output_list_[output_index];

            if (op_output_map.find(src_name) != op_output_map.end() &&
                    op_output_map.at(src_name).find(op_output_name) !=
                    op_output_map.at(src_name).end()) {
                // Intermidiate var already exists
                intermidiate_name = op_output_map.at(src_name).at(op_output_name);
                CHECK_KEY_IN_MAP(intermidiate_name, intermidiate_prev_op_map) <<
                    "prev_op is not set for existing intermidiate var '" << intermidiate_name << "'.";
            } else {
                // Create intermidiate var
                intermidiate_name = "V" + std::to_string(intermidiate_count);
                intermidiate_count++;

                // Set src operation as prev_op to intermidiate var
                CHECK_KEY_NOT_IN_MAP(intermidiate_name, intermidiate_prev_op_map) <<
                    "prev_op is already set for intermidiate var '" << intermidiate_name << "'.";
                intermidiate_prev_op_map.insert({intermidiate_name, src_name});

                // Add intermidiate var as an output to src operation
                if (op_output_map.find(src_name) != op_output_map.end()) {
                    op_output_map.at(src_name).insert({op_output_name, intermidiate_name});
                } else {
                    op_output_map.insert({src_name, {{op_output_name, intermidiate_name}}});
                }
            }

            // Connect intermidiate var to dst operation
            CHECK_KEY_IN_MAP(dst_name, operation_map_) <<
                "'" << dst_name << "' is not found as an operation.";
            CHECK(input_index < operation_map_.at(dst_name).input_list_.size()) <<
                "input_index '" << input_index << "' for operator '" <<
                operation_map_.at(dst_name).type_name_ << "' is out of bound.";

            const std::string op_input_name =
                operation_map_.at(dst_name).input_list_[input_index];

            // Add dst operation as a next_op to intermidiate var
            if (var_next_ops_map.find(intermidiate_name) != var_next_ops_map.end()) {
                CHECK_KEY_NOT_IN_VEC(dst_name, var_next_ops_map.at(intermidiate_name)) <<
                    "'" << dst_name << "' already exists in next_ops of '" << intermidiate_name << "'.";
                var_next_ops_map.at(intermidiate_name).push_back(dst_name);
            } else {
                var_next_ops_map.insert({intermidiate_name, {dst_name}});
            }

            // Add intermidiate var as an input to dst operation
            if (op_input_map.find(dst_name) != op_input_map.end()) {
                CHECK_KEY_NOT_IN_MAP(op_input_name, op_input_map.at(dst_name)) <<
                    "Input '" << op_input_name << "' for '" << dst_name << "' is already set.";
                op_input_map.at(dst_name).insert({op_input_name, intermidiate_name});
            } else {
                op_input_map.insert({dst_name, {{op_input_name, intermidiate_name}}});
            }
        }
    }

    for (std::pair<std::string, Operator> op_kv : operation_map_) {
        // Check missing operation inputs
        CHECK_KEY_IN_MAP(op_kv.first, op_input_map) <<
            "No input connected for operation '" << op_kv.first << "'.";
        for (std::string input_name : op_kv.second.input_list_) {
            CHECK_KEY_IN_MAP(input_name, op_input_map.at(op_kv.first)) <<
                "Input '" << input_name << "' for operation '" << op_kv.first << "' is not connected";
        }

        // Add missing operation outputs
        if (op_output_map.find(op_kv.first) == op_output_map.end()) {
            op_output_map.insert({op_kv.first, {}});
        }
        for (std::string output_name : op_kv.second.output_list_) {
            if (op_output_map.at(op_kv.first).find(output_name) ==
                    op_output_map.at(op_kv.first).end()) {
                // Create intermidiate var
                const std::string intermidiate_name = "V" + std::to_string(intermidiate_count);
                intermidiate_count++;

                // Set op as prev_op to intermidiate var
                CHECK_KEY_NOT_IN_MAP(intermidiate_name, intermidiate_prev_op_map) <<
                    "prev_op is already set for intermidiate var '" << intermidiate_name << "'.";
                intermidiate_prev_op_map.insert({intermidiate_name, op_kv.first});

                // Add intermidiate var as an output to op
                op_output_map.at(op_kv.first).insert({output_name, intermidiate_name});
            }
        }
    }

    // Construct HLIR
    HLIR hlir;

    // Construct and add all placeholder vars
    for (std::pair<std::string, Datatype> pld_kv : placeholder_map_) {
        CHECK_KEY_IN_MAP(pld_kv.first, var_next_ops_map) <<
            "No output connected for placeholder var '" << pld_kv.first << "'.";
        HLIR::Variable var(pld_kv.second, var_next_ops_map.at(pld_kv.first));
        hlir.AddVariable(pld_kv.first, var);
    }

    // Construct and add all constant vars
    for (std::pair<std::string, Data> const_kv : constant_map_) {
        CHECK_KEY_IN_MAP(const_kv.first, var_next_ops_map) <<
            "No output connected for placeholder var '" << const_kv.first << "'.";
        HLIR::Variable var(const_kv.second, var_next_ops_map.at(const_kv.first));
        hlir.AddVariable(const_kv.first, var);
    }

    // Construct and add all intermidiate vars
    for (std::pair<std::string, std::string> interm_kv : intermidiate_prev_op_map) {
        if (var_next_ops_map.find(interm_kv.first) != var_next_ops_map.end()) {
            HLIR::Variable var(interm_kv.second, var_next_ops_map.at(interm_kv.first));
            hlir.AddVariable(interm_kv.first, var);
        } else {
            HLIR::Variable var(interm_kv.second, {});
            hlir.AddVariable(interm_kv.first, var);
        }
    }

    // Construct and add all operations
    for (std::pair<std::string, Operator> op_kv : operation_map_) {
        CHECK_KEY_IN_MAP(op_kv.first, op_input_map) <<
            "No input connected for operation '" << op_kv.first << "'.";
        CHECK_KEY_IN_MAP(op_kv.first, op_output_map) <<
            "No output connected for operation '" << op_kv.first << "'.";
        HLIR::Operation op(op_kv.second.attr_val_map_,
                op_input_map.at(op_kv.first), op_output_map.at(op_kv.first));
        hlir.AddOperation(op_kv.first, op);
    }

    CHECK(hlir.IsValid()) << "Invalid HLIR instance.";
    return hlir;
}

} // namespace core
} // namespace tcc
