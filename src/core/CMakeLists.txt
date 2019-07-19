# Build core library
list(APPEND
    CORE_SRC
     ${TCC_INCLUDE_DIR}/tcc/core/common/data.h
     ${TCC_INCLUDE_DIR}/tcc/core/common/util.h
     ${TCC_INCLUDE_DIR}/tcc/core/common/logging.h
     ${TCC_INCLUDE_DIR}/tcc/core/hlir/hlir.h
     ${TCC_INCLUDE_DIR}/tcc/core/hlir/operation.h
     ${TCC_INCLUDE_DIR}/tcc/core/hlir/visualize.h
     ${TCC_INCLUDE_DIR}/tcc/core/hlir/lower.h
     ${TCC_INCLUDE_DIR}/tcc/core/llir/llir.h
     ${TCC_INCLUDE_DIR}/tcc/core/llir/expression.h
     ${TCC_INCLUDE_DIR}/tcc/core/llir/visualize.h
     ${TCC_INCLUDE_DIR}/tcc/core/llir/cgen.h
     hlir/hlir.cc
     hlir/operation.cc
     hlir/visualize.cc
     hlir/lower.cc
     llir/llir.cc
     llir/expression.cc
     llir/visualize.cc
     llir/cgen.cc)

add_library(core
    ${CORE_SRC})

target_include_directories(core
    PUBLIC
    ${TCC_ROOT_DIR}/extern/glog/src
    ${TCC_BINARY_DIR}/extern/glog)

target_link_libraries(core
    PUBLIC
    glog::glog)
