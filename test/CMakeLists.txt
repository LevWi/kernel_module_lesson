cmake_minimum_required(VERSION 3.0.2)

set(project_name string_buffer_test)

project(${project_name})

add_compile_options(-std=c++17)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_definitions("-D TEST_TYPES")
	
#include_directories(.)
add_executable(${project_name}
    ../src/string_buffer.h
    test.cpp
    )

#include_directories(.)
add_executable(read_write_to_device
    read_write_to_device.cpp
    )

target_link_libraries(read_write_to_device PRIVATE gtest pthread)

target_link_libraries(${project_name} PRIVATE gtest pthread)
