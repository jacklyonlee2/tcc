# Build tensorflow proto library
protobuf_generate_cpp(PROTO_SRCS PROTO_HEADERS
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
    ${PROTO_HEADERS}
    ${PROTO_SRCS})
