include("${PROJECT_SOURCE_DIR}/dynamicCMake/CMakeLists.txt")

dynamicCMake("main")

add_subdirectory(charStream)
add_subdirectory(lexer)
add_subdirectory(parser)
