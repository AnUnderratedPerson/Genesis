cmake_minimum_required(VERSION 3.10)

project(Genesis LANGUAGES CXX)

set(Genesis.SRC "${CMAKE_SOURCE_DIR}/src")
set(Genesis.INCLUDE "${CMAKE_SOURCE_DIR}/include")

add_executable(Genesis "${Genesis.SRC}/Genesis.cpp")

include("${CMAKE_SOURCE_DIR}/CMakeSource.cmake")