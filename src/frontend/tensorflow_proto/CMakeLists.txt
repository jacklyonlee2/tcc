# Build TensorFlow proto library
protobuf_generate_cpp(PROTO_SRC PROTO_HDR
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

add_library(tensorflow_proto
    ${PROTO_HDR}
    ${PROTO_SRC})

target_link_libraries(tensorflow_proto
    PRIVATE
    ${PROTOBUF_LIBRARIES})

