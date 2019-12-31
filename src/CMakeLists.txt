list(APPEND CORE_SRC
    ${TCC_INCLUDE_DIR}/tcc/common/logging.h
    ${TCC_INCLUDE_DIR}/tcc/common/datatype.h
    ${TCC_INCLUDE_DIR}/tcc/common/util.h
    ${TCC_INCLUDE_DIR}/tcc/hlir/ir.h
    ${TCC_INCLUDE_DIR}/tcc/hlir/visitor.h
    ${TCC_INCLUDE_DIR}/tcc/hlir/index_validator.h
    hlir/ir.cc
    hlir/visitor.cc
    hlir/index_validator.cc)

add_library(core
    ${CORE_SRC})

list(APPEND PROTO
    frontend/proto/attr_value.proto
    frontend/proto/node_def.proto
    frontend/proto/tensor.proto
    frontend/proto/versions.proto
    frontend/proto/function.proto
    frontend/proto/op_def.proto
    frontend/proto/tensor_shape.proto
    frontend/proto/graph.proto
    frontend/proto/resource_handle.proto
    frontend/proto/types.proto)

protobuf_generate_cpp(PROTO_SRC PROTO_HDR
    ${PROTO})

add_library(proto
    ${PROTO_HDR}
    ${PROTO_SRC})

target_link_libraries(proto
    PRIVATE
    ${PROTOBUF_LIBRARIES})

add_library(frontend
    ${TCC_INCLUDE_DIR}/tcc/frontend/parse.h
    ${TCC_INCLUDE_DIR}/tcc/frontend/op.h
    frontend/parse.cc
    frontend/op.cc)

target_include_directories(frontend
    PRIVATE
    ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(frontend
    PRIVATE
    core
    proto)
