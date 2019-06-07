# Build TCC
add_executable(tcc
    tcc.cc)

target_link_libraries(tcc
    PRIVATE
    core
    op
    parser
    gflags::gflags)

