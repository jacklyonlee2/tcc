# Build TensorFlow proto library
add_subdirectory(proto)

# Build parser library
add_library(parser
    ${TCC_INCLUDE_DIR}/tcc/parser/parser.h
    parser.cc)

target_include_directories(parser
    PRIVATE
    ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}) # "proto/*.pb.h" includes

target_link_libraries(parser
    PRIVATE
    core
    proto)
