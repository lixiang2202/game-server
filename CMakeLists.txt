cmake_minimum_required(VERSION 3.31)

project(game-server CXX)

include(cmake/CommonConfig.cmake)

set(Build_VERSION_MAJOR 0)
set(Build_VERSION_MINOR 1)

string(TIMESTAMP Build_TIMESTAMP "%Y-%m-%d %H:%M:%S")

configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/config.h.in
    ${CMAKE_SOURCE_DIR}/src/config.h
)


include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/util/include
)

set(LaunchSRCS 
    src/launch.cpp)

# add_subdirectory(src/third_party)
add_subdirectory(src/util)
add_subdirectory(src/platform)
add_subdirectory(test)

add_executable(${PROJECT_NAME} ${LaunchSRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE util)
target_link_libraries(${PROJECT_NAME} PRIVATE task_scheduler)
target_link_libraries(${PROJECT_NAME} PRIVATE time_manager)

# target_include_directories(${PROJECT_NAME}
#     PUBLIC
#         ${CMAKE_SOURCE_DIR}/src/util/include
# )


# if (MSVC)
#     # 定义要预处理的源文件路径
#     set(LAUNCH_SOURCE_FILE ${CMAKE_SOURCE_DIR}/src/launch.cpp)
#     add_custom_target(preprocess_launch
#         # 执行预处理命令
#         COMMAND ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS} /P ${LAUNCH_SOURCE_FILE}
#         # 设置工作目录
#         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#         # 注释信息
#         COMMENT "Preprocessing source file: ${LAUNCH_SOURCE_FILE}"
#         # 检查预处理文件是否生成
#         COMMAND ${CMAKE_COMMAND} -E echo "Checking if preprocessed file was generated..."
#         COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "Preprocessed file: ${LAUNCH_SOURCE_FILE}.i"
#         COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "Preprocessing completed."
#     )
# endif()