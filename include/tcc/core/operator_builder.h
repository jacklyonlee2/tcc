#ifndef TCC_OPERATOR_BUILDER_H
#define TCC_OPERATOR_BUILDER_H

#include <string>

namespace tcc {
namespace core {

class Operator;

class OperatorBuilder {
    public:
        OperatorBuilder(std::string type_name);

        OperatorBuilder& Input(std::string input_name);
        OperatorBuilder& Output(std::string output_name);

    private:
        const std::string type_name_;

    friend class Operator;
};

} // namespace core
} // namespace tcc

#endif // TCC_OPERATOR_BUILDER_H