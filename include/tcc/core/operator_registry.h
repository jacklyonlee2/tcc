#ifndef TCC_OPERATOR_REGISTRY_H
#define TCC_OPERATOR_REGISTRY_H

#include <unordered_map>

#include "tcc/core/operator.h"

namespace tcc {
namespace core {

class OperatorRegistry {
    public:
        static bool GetTest(std::string type_name);

    private:
        static void Register(std::string type_name, Operator op);

        static std::unordered_map<std::string, Operator> registry_;

    friend class Operator;
};

} // namespace core
} // namespace tcc

#endif // TCC_OPERATOR_REGISTRY_H
