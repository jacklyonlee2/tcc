# Build core library
list(APPEND
    CORE_SRC
    ${TCC_INCLUDE_DIR}/tcc/core/compiler.h
    ${TCC_INCLUDE_DIR}/tcc/core/context.h
    ${TCC_INCLUDE_DIR}/tcc/core/data.h
    ${TCC_INCLUDE_DIR}/tcc/core/datatype.h
    ${TCC_INCLUDE_DIR}/tcc/core/hlir.h
    ${TCC_INCLUDE_DIR}/tcc/core/logging.h
    ${TCC_INCLUDE_DIR}/tcc/core/operator.h
    ${TCC_INCLUDE_DIR}/tcc/core/operator_registry.h
    compiler.cc
    context.cc
    hlir.cc
    operator.cc
    operator_registry.cc)

add_library(core
    ${CORE_SRC})

target_compile_options(core
    PRIVATE
    "-fPIC") # linking with shared library op

target_include_directories(core
    PUBLIC
    ${TCC_ROOT_DIR}/extern/glog/src
    ${TCC_BINARY_DIR}/extern/glog)

target_link_libraries(core
    PUBLIC
    glog::glog)
