cmake_minimum_required(VERSION 3.10)

project(GenesisTesting)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(GenesisTesting ${SOURCE_DIR}/Genesis.cpp)

target_compile_features(GenesisTesting PUBLIC cxx_std_17)

target_sources(GenesisTesting PUBLIC
    # PRE CODE-PROCESSOR
    ${INCLUDE_DIR}/Processing/Processor.hpp
    ${INCLUDE_DIR}/Processing/Lexer.hpp
    ${INCLUDE_DIR}/Processing/TokenClass.hpp

    # INTERNAL UTILITIES
    ${INCLUDE_DIR}/Util/Util.hpp
)