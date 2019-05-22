# Build core library
add_library(core
    operator_builder.cc
    operator.cc
    ${TCC_INCLUDE_DIR}/tcc/core/operator_builder.h
    ${TCC_INCLUDE_DIR}/tcc/core/operator.h)

target_link_libraries(core
    PUBLIC
    glog::glog)
