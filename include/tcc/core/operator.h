#ifndef TCC_OPERATOR_H
#define TCC_OPERATOR_H

#include "tcc/core/operator_builder.h"

namespace tcc {
namespace core {

class Operator {
    public:
        Operator() {}
        Operator(OperatorBuilder& builder);
};

} // namespace core
} // namespace tcc

#endif // TCC_OPERATOR_H
