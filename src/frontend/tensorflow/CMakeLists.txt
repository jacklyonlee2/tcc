# Build TensorFlow proto library
add_subdirectory(proto)

# Build TensorFlow parser library
add_library(tensorflow_parser
    parser.cc)

target_include_directories(tensorflow_parser
    PRIVATE
    ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}) # "proto/*.pb.h" includes

target_link_libraries(tensorflow_parser
    PRIVATE
    core
    tensorflow_proto)
