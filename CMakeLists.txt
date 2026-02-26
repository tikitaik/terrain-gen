cmake_minimum_required(VERSION 3.10)

project(terrain-gen)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall)

set(CMAKE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(terrain-gen 
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/glad.c 
    ${CMAKE_SOURCE_DIR}/src/camera.cpp
    ${CMAKE_SOURCE_DIR}/src/shader.cpp)

target_include_directories(terrain-gen PUBLIC ${CMAKE_SOURCE_DIR}/include)

if (APPLE)
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH}/mac)
elseif (UNIX)
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH}/linux)
elseif(WIN32)
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH}/win)
endif()

find_library(GLFW NAMES glfw glfw3 REQUIRED)

target_link_libraries(terrain-gen PRIVATE ${GLFW})

file(COPY ${CMAKE_SOURCE_DIR}/src/shaders DESTINATION ${CMAKE_BINARY_DIR})
