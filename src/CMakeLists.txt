list(APPEND CORE_SRC
    ${TCC_INCLUDE_DIR}/tcc/common/logging.h
    ${TCC_INCLUDE_DIR}/tcc/common/datatype.h
    ${TCC_INCLUDE_DIR}/tcc/common/util.h
    ${TCC_INCLUDE_DIR}/tcc/hlir/ir.h
    ${TCC_INCLUDE_DIR}/tcc/hlir/visitor.h
    ${TCC_INCLUDE_DIR}/tcc/hlir/index_validator.h
    ${TCC_INCLUDE_DIR}/tcc/hlir/dot_printer.h
    ${TCC_INCLUDE_DIR}/tcc/hlir/lower.h
    ${TCC_INCLUDE_DIR}/tcc/llir/ir.h
    hlir/ir.cc
    hlir/visitor.cc
    hlir/index_validator.cc
    hlir/dot_printer.cc
    hlir/lower.cc
    llir/ir.cc)

add_library(core
    ${CORE_SRC})

add_subdirectory(frontend/proto tensorflow)

add_library(frontend
    ${TCC_INCLUDE_DIR}/tcc/frontend/parser.h
    ${TCC_INCLUDE_DIR}/tcc/frontend/op.h
    frontend/parser.cc
    frontend/op.cc)

target_include_directories(frontend
    PRIVATE
    ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(frontend
    PRIVATE
    core
    tensorflow_proto)
