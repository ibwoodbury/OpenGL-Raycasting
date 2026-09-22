cmake_minimum_required(VERSION 3.16)

project(OpenGLTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(OpenGLTest
    src/main.cpp
)

target_link_libraries(OpenGLTest
    PRIVATE
        glfw
        OpenGL::GL
)

