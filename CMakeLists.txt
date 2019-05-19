cmake_minimum_required(VERSION 3.10)

project(TCC VERSION 0.0
            DESCRIPTION "End-to-End Ahead-of-Time ML Compiler"
            LANGUAGES CXX)

add_compile_options(-Wall -Wextra -Wpedantic)

# Set variables
set(TCC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Check dependencies
find_package(Protobuf REQUIRED)

# include headers
include_directories(${TCC_INCLUDE_DIR})

# Build libraries
add_subdirectory(src)

# Build executables
add_subdirectory(apps)
