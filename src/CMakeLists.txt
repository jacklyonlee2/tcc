list(APPEND CORE_SRC
    ${TCC_INCLUDE_DIR}/tcc/common/logging.h
    ${TCC_INCLUDE_DIR}/tcc/common/datatype.h
    ${TCC_INCLUDE_DIR}/tcc/common/util.h
    ${TCC_INCLUDE_DIR}/tcc/core/ir.h
    ${TCC_INCLUDE_DIR}/tcc/core/ir_visitor.h
    ${TCC_INCLUDE_DIR}/tcc/core/ir_util.h
    ${TCC_INCLUDE_DIR}/tcc/core/ir_printer.h
    ${TCC_INCLUDE_DIR}/tcc/core/ir_codegen.h
    core/ir.cc
    core/ir_visitor.cc
    core/ir_util.cc
    core/ir_printer.cc
    core/ir_codegen.cc)

add_library(core
    ${CORE_SRC})

add_subdirectory(frontend/proto proto)

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
    proto)
