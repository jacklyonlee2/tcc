#ifndef TCC_OPERATOR_H
#define TCC_OPERATOR_H

#include "tcc/core/common/operator_builder.h"

namespace tcc {
namespace core {
namespace common {

class Operator {
    public:
        Operator() {}
        Operator(OperatorBuilder builder);
};

} // namespace common
} // namespace core
} // namespace tcc

#endif // TCC_OPERATOR_H
