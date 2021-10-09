target_sources(lexer
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/token.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/token.h
    ${CMAKE_CURRENT_SOURCE_DIR}/charStream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/charStream.h)
#

target_include_directories(lexer PUBLIC ${CMAKE_CURRENT_LIST_DIR})