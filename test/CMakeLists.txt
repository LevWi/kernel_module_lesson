set(project_name string_buffer_test)

project(${project_name})

add_definitions("-D TEST_TYPES")
	
#include_directories(.)
add_executable(${project_name}
    #Todo
    )

target_link_libraries(${project_name} PRIVATE gtest)