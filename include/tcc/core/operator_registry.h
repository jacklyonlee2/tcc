#ifndef TCC_OPERATOR_REGISTRY_H
#define TCC_OPERATOR_REGISTRY_H

#include <unordered_map>
#include <memory>

#include "tcc/core/operator.h"

namespace tcc {
namespace core {

class OperatorRegistry {
    public:
        typedef std::shared_ptr<std::unordered_map<std::string, Operator>> RegistryPtr;

        static Operator Instantiate(std::string type_name);

    private:
        static void Register(std::string type_name, Operator& op);

        static RegistryPtr registry_;

    friend class Operator;
};

} // namespace core
} // namespace tcc

#endif // TCC_OPERATOR_REGISTRY_H
