target_sources(regex
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/regex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/regex.h)
#

target_include_directories(regex PUBLIC ${CMAKE_CURRENT_LIST_DIR})
