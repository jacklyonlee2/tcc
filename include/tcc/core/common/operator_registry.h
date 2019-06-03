#ifndef TCC_OPERATOR_REGISTRY_H
#define TCC_OPERATOR_REGISTRY_H

#include <unordered_map>
#include <memory>

#include "tcc/core/common/operator.h"

namespace tcc {
namespace core {
namespace common {

class OperatorRegistry {
    public:
        typedef std::shared_ptr<std::unordered_map<std::string, Operator>> RegistryPtr;

        static bool Registered(const std::string type_name);
        static Operator Instantiate(const std::string type_name);

    private:
        static void Register(std::string type_name, Operator& op);

        static RegistryPtr registry_;

    friend class Operator;
};

} // namespace common
} // namespace core
} // namespace tcc

#endif // TCC_OPERATOR_REGISTRY_H
