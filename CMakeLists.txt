cmake_minimum_required(VERSION 3.10)

project(TCC VERSION 0.0
            DESCRIPTION "End-to-End Ahead-of-Time ML Compiler"
            LANGUAGES CXX)

# Compiler options
add_compile_options(-Wall -Wextra -Wpedantic)

# Global variables
set(TCC_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TCC_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(TCC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(WITH_GFLAGS OFF CACHE BOOL "Use gflags") # Disable gflags when building glog

# Global includes
include_directories(${TCC_INCLUDE_DIR})

# Build dependencies
find_package(Protobuf 3.7.1 REQUIRED) # protobuf-3.7.1
add_subdirectory(extern/gflags) # gflags-2.2.0
add_subdirectory(extern/glog) # glog-0.4.0
add_subdirectory(extern/googletest) # googletest-1.8.1

# Build tcc
add_subdirectory(src) # libraries
add_subdirectory(app) # executables
