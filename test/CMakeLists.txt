cmake_minimum_required(VERSION 3.10)

project(game-svr-test CXX)

file(GLOB_RECURSE TEST_SRC_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

set(GAME_SERVER_LIBS 
    util 
    task_scheduler 
    time_manager)

foreach(src ${TEST_SRC_LIST})
    get_filename_component(bin_name ${src} NAME_WE)
    add_executable(${bin_name} ${src})
    target_link_libraries(${bin_name} PRIVATE ${GAME_SERVER_LIBS})
    set_target_properties(${bin_name} PROPERTIES FOLDER "test")
endforeach()

