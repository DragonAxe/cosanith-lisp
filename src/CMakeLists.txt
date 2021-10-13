target_sources(main
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
#

target_include_directories(main PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(regex)
