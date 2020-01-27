add_executable(tcc
    tcc.cc)

target_link_libraries(tcc
    PRIVATE
    frontend
    core)
