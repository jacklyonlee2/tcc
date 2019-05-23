# Build ops library
add_library(ops
    SHARED
    ops.cc)

target_link_libraries(ops
    PRIVATE
    core)
