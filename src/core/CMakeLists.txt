# Build core library
list(APPEND
    COMMON_SRCS
    ${TCC_INCLUDE_DIR}/tcc/core/common/data.h
    ${TCC_INCLUDE_DIR}/tcc/core/common/datatype.h
    ${TCC_INCLUDE_DIR}/tcc/core/common/operator_registry.h
    ${TCC_INCLUDE_DIR}/tcc/core/common/operator_builder.h
    ${TCC_INCLUDE_DIR}/tcc/core/common/operator.h
    ${TCC_INCLUDE_DIR}/tcc/core/common/register_op.h
    ./common/data.cc
    ./common/operator_registry.cc
    ./common/operator_builder.cc
    ./common/operator.cc)

list(APPEND
    HLIR_SRCS
    ${TCC_INCLUDE_DIR}/tcc/core/hlir/hlir.h
    ./hlir/hlir.cc)

list(APPEND
    LLIR_SRCS
    ${TCC_INCLUDE_DIR}/tcc/core/llir/llir.h
    ./llir/llir.cc)

add_library(core
    ${COMMON_SRCS}
    ${HLIR_SRCS}
    ${LLIR_SRCS})

target_compile_options(core
    PRIVATE
    "-fPIC") # allows shared ops libraries to work

target_link_libraries(core
    PRIVATE
    util)
