# Build util library
add_library(util
    ${TCC_INCLUDE_DIR}/tcc/util/logging.h)

target_link_libraries(util
    PUBLIC
    glog::glog)

# Build other libraries
add_subdirectory(core)
add_subdirectory(ops)
add_subdirectory(parser)
