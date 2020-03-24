cmake_minimum_required(VERSION 3.10)

project(tcc
    VERSION 0.0
    DESCRIPTION "End-to-End Ahead-of-Time ML Compiler"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-Wall -Wextra -Wpedantic -g)

set(TCC_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TCC_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(TCC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(Protobuf 3.7.1 REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

include_directories(${TCC_INCLUDE_DIR})

add_subdirectory(src)
add_subdirectory(app)
add_subdirectory(test)
