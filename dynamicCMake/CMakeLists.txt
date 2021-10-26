function(dynamicCMake MyModuleName)

    file(GLOB MySources *.cpp)
    file(GLOB MyHeaders *.h)

    # Dynamicly generate module name if not defined by user
    if (MyModuleName MATCHES "^$")
        get_filename_component(MyModuleName "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
        if (MyModuleName MATCHES "^src$")
            set(MyModuleName main)
        endif ()
    endif ()

    target_sources("${MyModuleName}"
            PRIVATE
            ${MySources}
            ${MyHeaders})

    target_include_directories("${MyModuleName}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    #
    #
    #

    string(REPLACE "${PROJECT_SOURCE_DIR}" "" MyPath "${CMAKE_CURRENT_SOURCE_DIR}")

    set(FolderNamespace "")
    set(NamespaceGlue "")
    while(NOT MyPath STREQUAL "/")
        get_filename_component(FolderName "${MyPath}" NAME)
        get_filename_component(MyPath "${MyPath}" DIRECTORY)
        string(PREPEND FolderNamespace "${FolderName}${NamespaceGlue}")
        set(NamespaceGlue "::")
    endwhile()

    # target_compile_options("${MyModuleName}" PRIVATE "-DFOLDER_NAMESPACE=${FolderNamespace}")

    list(LENGTH MySources MySrcSize)
    list(LENGTH MyHeaders MyHeaderSize)
    message("Found MyModule: ${MyModuleName}, cpp=${MySrcSize}, h=${MyHeaderSize}, namespace=${FolderNamespace}")

endfunction()
