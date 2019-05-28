# Build core library
list(APPEND
    OPERATOR_SRCS
    ${TCC_INCLUDE_DIR}/tcc/core/operator/operator_registry.h
    ${TCC_INCLUDE_DIR}/tcc/core/operator/operator_builder.h
    ${TCC_INCLUDE_DIR}/tcc/core/operator/operator.h
    ${TCC_INCLUDE_DIR}/tcc/core/operator/register_op.h
    ./operator/operator_registry.cc
    ./operator/operator_builder.cc
    ./operator/operator.cc)

list(APPEND
    HLIR_SRCS
    ${TCC_INCLUDE_DIR}/tcc/core/hlir/hlir.h
    ./hlir/hlir.cc)

list(APPEND
    LLIR_SRCS
    ${TCC_INCLUDE_DIR}/tcc/core/llir/llir.h
    ./llir/llir.cc)

add_library(core
    ${OPERATOR_SRCS}
    ${HLIR_SRCS}
    ${LLIR_SRCS})

target_compile_options(core
    PRIVATE
    "-fPIC") # allows shared ops libraries to work

target_link_libraries(core
    PRIVATE
    util)
