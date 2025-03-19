cmake_minimum_required(VERSION 3.31)

project(game-server CXX)

include(cmake/CommonConfig.cmake)

set(LaunchSRCS 
    src/launch.cpp)

# add_subdirectory(src/third_party)
add_subdirectory(src/util)

add_executable(${PROJECT_NAME} ${LaunchSRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE util)

# target_include_directories(${PROJECT_NAME}
#     PUBLIC
#         ${CMAKE_SOURCE_DIR}/include
# )