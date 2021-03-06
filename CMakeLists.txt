cmake_minimum_required(VERSION 3.1)
project(calculator)

set(PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(${MSVC})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
else()
    add_definitions("-std=c++20")
endif()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

if( NOT ${WIN32} )
    add_custom_command(
        OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/a.out
        COMMAND clang ${CMAKE_SOURCE_DIR}/files/lisp.ll -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/a.out
        DEPENDS ${CMAKE_SOURCE_DIR}/files/lisp.ll)

    add_custom_target(
        lispll
        ALL
        DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/a.out)
endif()

add_library(charStream)
add_library(lexer)
target_link_libraries(lexer charStream)
add_library(parser)
target_link_libraries(parser lexer)
add_executable(main)
target_link_libraries(main lexer parser ${CONAN_LIBS})

add_subdirectory(src)
