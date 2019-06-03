#ifndef TCC_OPERATOR_BUILDER_H
#define TCC_OPERATOR_BUILDER_H

#include <string>

namespace tcc {
namespace core {
namespace common {

class Operator;

class OperatorBuilder {
    public:
        OperatorBuilder(const std::string type_name);

        OperatorBuilder& Attr(const std::string attr_name);
        OperatorBuilder& Input(const std::string input_name);
        OperatorBuilder& Output(const std::string output_name);

    private:
        const std::string type_name_;

    friend class Operator;
};

} // namespace common
} // namespace core
} // namespace tcc

#endif // TCC_OPERATOR_BUILDER_H
