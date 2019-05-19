# Build tensorflow proto library
protobuf_generate_cpp(PROTO_SRC PROTO_HEADER
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
    ${PROTO_HEADER}
    ${PROTO_SRC})
