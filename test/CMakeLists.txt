cmake_minimum_required(VERSION 3.10)

project(game-svr-test CXX)

# file(GLOB_RECURSE TEST_SRC_LIST
#     ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

set(TEST_SRC_LIST
    time_manager_test.cpp
    any_test.cpp)

foreach(src ${TEST_SRC_LIST})
    string(REPLACE ".cpp" "" test_name ${src})
    list(GET test_name -1 bin_name)
    add_executable(${bin_name} ${src})
    target_link_libraries(${bin_name} PRIVATE 
        util
        task_scheduler
        time_manager)
endforeach()

