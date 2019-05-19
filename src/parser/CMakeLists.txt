# Build tensorflow proto library
add_subdirectory(protos)

# Build parser library
add_library(parser
    parser.cc
    ${TCC_INCLUDE_DIR}/tcc/parser/parser.h)

target_include_directories(parser
    PRIVATE
    ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}) # "proto/*.pb.h" includes

target_link_libraries(parser
    PRIVATE
    protos)
