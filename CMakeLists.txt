cmake_minimum_required(VERSION 2.13)
project(calculator)

add_definitions("-std=c++20")
# set_property(PROPERTY CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_custom_command(
    OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/a.out
    COMMAND clang ${CMAKE_SOURCE_DIR}/files/lisp.ll -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/a.out
    DEPENDS ${CMAKE_SOURCE_DIR}/files/lisp.ll)

add_custom_target(
    lispll
    ALL
    DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/a.out)
#

add_library(lexer)
add_library(parser)
target_link_libraries(parser lexer)
add_executable(main)
target_link_libraries(main lexer parser ${CONAN_LIBS})

add_subdirectory(src)


