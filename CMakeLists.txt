cmake_minimum_required(VERSION 3.10)

project(TCC VERSION 0.0
            DESCRIPTION "End-to-End Ahead-of-Time ML Compiler"
            LANGUAGES CXX)

# -- Compiler options --
add_compile_options(-Wall -Wextra -Wpedantic)

# -- Set variables --
set(TCC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# -- Build dependencies --
find_package(Protobuf 3.7.1 REQUIRED) # protobuf-3.7.1

set(WITH_GFLAGS OFF CACHE BOOL "Use gflags")
set(BUILD_TESTING OFF CACHE BOOL "Enable unittesting")
add_subdirectory(extern/glog) # glog-0.4.0

add_subdirectory(extern/googletest) # googletest-1.8.1

# -- Build TCC --
include_directories(${TCC_INCLUDE_DIR})
add_subdirectory(src) # libraries
add_subdirectory(apps) # executables
