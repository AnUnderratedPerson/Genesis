cmake_minimum_required(VERSION 3.10)

project(Genesis LANGUAGES CXX)

set(Genesis.SRC "${CMAKE_SOURCE_DIR}/src")
set(Genesis.INCLUDE "${CMAKE_SOURCE_DIR}/include")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-std=c++17)

add_executable(Genesis "${Genesis.SRC}/Genesis.cpp")
target_compile_features(Genesis PUBLIC cxx_std_17)

include("${CMAKE_SOURCE_DIR}/CMakeSource.cmake")