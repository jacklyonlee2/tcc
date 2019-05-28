# Build tensorflow proto library
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
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

add_library(protos
    ${PROTO_HDRS}
    ${PROTO_SRCS})

target_link_libraries(protos
    PRIVATE
    ${PROTOBUF_LIBRARIES})
