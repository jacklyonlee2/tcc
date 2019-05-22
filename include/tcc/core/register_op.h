#ifndef TCC_REGISTER_OP_H
#define TCC_REGISTER_OP_H

#include "tcc/core/operator_builder.h"
#include "tcc/core/operator_registry.h"

#define REGISTER_OP(type_name) REGISTER_OP_UNIQ_HELPER(__COUNTER__, type_name)
#define REGISTER_OP_UNIQ_HELPER(ctr, type_name) REGISTER_OP_UNIQ(ctr, type_name)
#define REGISTER_OP_UNIQ(ctr, type_name) \
    static ::tcc::core::Operator registered_op_##ctr = \
        ::tcc::core::OperatorBuilder(type_name)

#endif // TCC_REGISTER_OP_H
