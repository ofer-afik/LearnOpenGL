cmake_minimum_required(VERSION 3.16)
project(LearnOpenGL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

option(LEARNOPENGL_FETCH_STATIC_GLFW "Build GLFW statically from source" OFF)
option(LEARNOPENGL_FETCH_STATIC_ASSIMP "Build Assimp and its bundled zlib statically from source" OFF)
option(LEARNOPENGL_STATIC_CXX_RUNTIME "Statically link the GNU C/C++ runtime libraries" OFF)

if(LEARNOPENGL_FETCH_STATIC_GLFW)
    include(FetchContent)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(glfw)
else()
    find_package(glfw3 3.3 REQUIRED)
endif()

if(LEARNOPENGL_FETCH_STATIC_ASSIMP)
    include(FetchContent)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
    if(APPLE)
        set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
    else()
        set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
    endif()
    set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
    set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.4.3
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(assimp)
else()
    find_package(assimp REQUIRED)
endif()

add_executable(build
    src/main.cpp
    glad.c
    src/model.hpp
)

target_include_directories(build PRIVATE ./include)

target_link_libraries(build PRIVATE
    glfw
    assimp::assimp
    OpenGL::GL
    ${CMAKE_DL_LIBS}
)

if(LEARNOPENGL_STATIC_CXX_RUNTIME AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(build PRIVATE -static-libgcc -static-libstdc++)
endif()

add_custom_target(stage_assets
    COMMAND ${CMAKE_COMMAND} -E remove_directory
        "${CMAKE_CURRENT_BINARY_DIR}/src/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/src/assets"
        "${CMAKE_CURRENT_BINARY_DIR}/src/assets"
)
add_dependencies(build stage_assets)
