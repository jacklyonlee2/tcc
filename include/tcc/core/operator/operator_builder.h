#ifndef TCC_OPERATOR_BUILDER_H
#define TCC_OPERATOR_BUILDER_H

#include <string>

namespace tcc {
namespace core {
namespace op {

class Operator;

class OperatorBuilder {
    public:
        OperatorBuilder(std::string type_name);

        OperatorBuilder& Attr(std::string attr_name);
        OperatorBuilder& Input(std::string input_name);
        OperatorBuilder& Output(std::string output_name);

    private:
        const std::string type_name_;

    friend class Operator;
};

} // namespace op
} // namespace core
} // namespace tcc

#endif // TCC_OPERATOR_BUILDER_H
