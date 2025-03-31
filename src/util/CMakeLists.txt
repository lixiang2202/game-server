cmake_minimum_required(VERSION 3.31)

project(util)

include(${CMAKE_SOURCE_DIR}/cmake/CommonConfig.cmake)


file(GLOB_RECURSE ALL_SRCS
    src/*.cpp
) 
set(UtilSRCS ${ALL_SRCS}) 

add_library(${PROJECT_NAME} STATIC ${UtilSRCS})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "until")

target_include_directories(${PROJECT_NAME} 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
