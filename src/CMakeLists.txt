list(APPEND CORE_SRC
    ${TCC_INCLUDE_DIR}/tcc/common/logging.h
    ${TCC_INCLUDE_DIR}/tcc/common/datatype.h
    ${TCC_INCLUDE_DIR}/tcc/common/util.h
    ${TCC_INCLUDE_DIR}/tcc/affn/ir.h
    ${TCC_INCLUDE_DIR}/tcc/affn/ir_visitor.h
    ${TCC_INCLUDE_DIR}/tcc/affn/index_validator.h
    ${TCC_INCLUDE_DIR}/tcc/affn/ir_printer.h
    ${TCC_INCLUDE_DIR}/tcc/affn/ir_lower.h
    ${TCC_INCLUDE_DIR}/tcc/lang/ir.h
    affn/ir.cc
    affn/ir_visitor.cc
    affn/index_validator.cc
    affn/ir_printer.cc
    affn/ir_lower.cc
    lang/ir.cc)

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
