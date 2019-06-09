#ifndef TCC_OPERATOR_H
#define TCC_OPERATOR_H

#include <string>
#include <vector>
#include <unordered_map>

#include "tcc/core/data.h"

namespace tcc {
namespace core {

class Operator;

class OperatorBuilder {
    public:
        OperatorBuilder(const std::string type_name);

        OperatorBuilder& Attr(const std::string attr_name, const Datatype datatype);
        OperatorBuilder& Input(const std::string input_name);
        OperatorBuilder& Output(const std::string output_name);

    private:
        const std::string type_name_;
        std::unordered_map<std::string, Datatype> attr_type_map_;
        std::vector<std::string> input_list_;
        std::vector<std::string> output_list_;

    friend class Operator;
};

class Operator {
    public:
        Operator(OperatorBuilder builder);

        void SetAttr(std::string attr_name, Data attr_val);

    private:
        const std::string type_name_;
        const std::unordered_map<std::string, Datatype> attr_type_map_;
        const std::vector<std::string> input_list_;
        const std::vector<std::string> output_list_;

        std::unordered_map<std::string, Data> attr_val_map_;

    friend class ParserContext;
};

} // namespace core
} // namespace tcc

#define REGISTER_OP(type_name) REGISTER_OP_UNIQ_HELPER(__COUNTER__, type_name)
#define REGISTER_OP_UNIQ_HELPER(ctr, type_name) REGISTER_OP_UNIQ(ctr, type_name)
#define REGISTER_OP_UNIQ(ctr, type_name) \
    static ::tcc::core::Operator registered_op_##ctr = \
        ::tcc::core::OperatorBuilder(type_name)

#endif // TCC_OPERATOR_H
