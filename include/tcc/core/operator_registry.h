#ifndef TCC_OPERATOR_REGISTRY_H
#define TCC_OPERATOR_REGISTRY_H

#include <unordered_map>
#include <memory>

#include "tcc/core/operator.h"

namespace tcc {
namespace core {

class ParserContext;

class OperatorRegistry {
    private:
        static void Register(std::string type_name, Operator& op);

        static std::shared_ptr<std::unordered_map<std::string, Operator>> registry_;

    friend class Operator;
    friend class ParserContext;
};

} // namespace core
} // namespace tcc

#endif // TCC_OPERATOR_REGISTRY_H
