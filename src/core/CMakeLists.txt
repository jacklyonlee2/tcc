# Build core library
add_library(core
    ${TCC_INCLUDE_DIR}/tcc/core/operator_registry.h
    ${TCC_INCLUDE_DIR}/tcc/core/operator_builder.h
    ${TCC_INCLUDE_DIR}/tcc/core/operator.h
    ${TCC_INCLUDE_DIR}/tcc/core/register_op.h
    operator_registry.cc
    operator_builder.cc
    operator.cc)

target_compile_options(core
    PRIVATE
    "-fPIC") # allows shared ops libraries to work

target_link_libraries(core
    PRIVATE
    util)
