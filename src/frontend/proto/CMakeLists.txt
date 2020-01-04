list(APPEND PROTO
    attr_value.proto
    node_def.proto
    tensor.proto
    versions.proto
    function.proto
    op_def.proto
    tensor_shape.proto
    graph.proto
    resource_handle.proto
    types.proto)

protobuf_generate_cpp(PROTO_SRC PROTO_HDR
    ${PROTO})

add_library(proto
    ${PROTO_HDR}
    ${PROTO_SRC})

target_link_libraries(proto
    PRIVATE
    ${PROTOBUF_LIBRARIES})
