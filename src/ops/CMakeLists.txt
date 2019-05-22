# Build ops library
add_library(ops
    ops.cc)

target_link_libraries(ops
    PRIVATE
    core)
