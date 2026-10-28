cmake_minimum_required(VERSION 3.16)
project(LearnOpenGL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_executable(linux_build
    src/main.cpp
    glad.c
)

target_include_directories(linux_build PRIVATE ./include)

target_link_libraries(linux_build PRIVATE
    glfw
    OpenGL::GL
    ${CMAKE_DL_LIBS}
    pthread
)