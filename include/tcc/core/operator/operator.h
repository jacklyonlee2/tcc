#ifndef TCC_OPERATOR_H
#define TCC_OPERATOR_H

#include "tcc/core/operator/operator_builder.h"

namespace tcc {
namespace core {
namespace op {

class Operator {
    public:
        Operator() {}
        Operator(OperatorBuilder& builder);
};

} // namespace op
} // namespace core
} // namespace tcc

#endif // TCC_OPERATOR_H
