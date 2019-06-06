#ifndef TCC_OPERATOR_BUILDER_H
#define TCC_OPERATOR_BUILDER_H

#include <string>
#include <vector>
#include <unordered_map>

#include "tcc/core/common/datatype.h"

namespace tcc {
namespace core {
namespace common {

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

} // namespace common
} // namespace core
} // namespace tcc

#endif // TCC_OPERATOR_BUILDER_H
