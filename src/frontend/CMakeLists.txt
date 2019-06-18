# Build TensorFlow proto library
add_subdirectory(tensorflow_proto)

# Build frontend library
add_library(frontend
    ${TCC_INCLUDE_DIR}/tcc/frontend/frontend.h
    tensorflow.cc)

target_include_directories(frontend
    PRIVATE
    ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}) # "tensorflow_proto/*.pb.h" includes

target_link_libraries(frontend
    PRIVATE
    core
    tensorflow_proto)
