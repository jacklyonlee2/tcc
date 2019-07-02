# Build TensorFlow parser library
add_subdirectory(tensorflow)

# Build frontend library
add_library(frontend
    ${TCC_INCLUDE_DIR}/tcc/frontend/frontend.h)

target_link_libraries(frontend
    PRIVATE
    core
    tensorflow_parser)
