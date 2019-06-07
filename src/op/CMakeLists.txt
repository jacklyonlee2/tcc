# Build op library
add_library(op
    SHARED
    ops.cc)

target_link_libraries(op
    PRIVATE
    core)
