
# CMake Common Configuration File
set(CMAKE_SUPPRESS_REGENERATION TRUE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# 设置默认构建类型（如果没有指定）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

# 根据构建类型设置不同的编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")   
    if(MSVC)
        add_compile_options(/Zi /Od)  # MSVC 的调试信息和不优化选项
    else()
        add_compile_options(-g -O0)   # GCC/Clang 的调试信息和不优化选项
    endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(/O2)      # MSVC 的优化选项
    else()
        add_compile_options(-O3)      # GCC/Clang 的优化选项
    endif()
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4)
    # 忽略特定的警告
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4100")
endif()