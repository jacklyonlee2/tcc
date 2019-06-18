# Build op library
add_library(op
    SHARED
    tensorflow.cc)

target_link_libraries(op
    PRIVATE
    core)
