add_executable(nn_test
    nn_test.cc)

target_link_libraries(nn_test
    PRIVATE
    frontend
    core
    dl)

add_executable(op_test
    op_test.cc)

target_link_libraries(op_test
    PRIVATE
    frontend
    core
    dl)
