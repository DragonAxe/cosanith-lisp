target_sources(parser
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ast.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ast.h
    ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parser.h)
#

target_include_directories(parser PUBLIC ${CMAKE_CURRENT_LIST_DIR})