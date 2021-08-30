cmake_minimum_required (VERSION 3.15)

### Macro ###
macro(macro_example) # ${ARGN} link libraries
    set(prefix EXAMPLE)
    set(flags COPY_LIB NONE)
    set(singleValues EXE FILE)
    set(multiValues DEPS LINKS)

    include(CMakeParseArguments)
    cmake_parse_arguments(${prefix} "${flags}" "${singleValues}" "${multiValues}" ${ARGN})
    #message("${EXE} - ${FILE} - ${DEPS} - ${LINKS}")
    #message("EXE====${EXAMPLE_EXE}")
    #message("IS NONE: ${EXAMPLE_NONE}")
    #message("IS CPLIB: ${EXAMPLE_COPY_LIB}")
    #message("EXE: ${EXAMPLE_EXE}")
    #message("FILE: ${EXAMPLE_FILE}")
    #message("DEP: ${EXAMPLE_DEPS}")
    #message("LINKS: ${EXAMPLE_LINKS}")
    add_executable (${EXAMPLE_EXE} ${EXAMPLE_FILE})
    add_dependencies(${EXAMPLE_EXE} ${EXAMPLE_DEPS}) 
    target_link_libraries(${EXAMPLE_EXE} ${EXAMPLE_LINKS})
    if(${EXAMPLE_COPY_LIB})
        #message("===============EXAMPLE COPY LIB")
        foreach(TF IN ITEMS ${EXAMPLE_DEPS})
            #message("++++ TF= ${TF} ++++")
            add_custom_command(
                TARGET ${EXAMPLE_EXE} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${TF}> $<TARGET_FILE_DIR:${EXAMPLE_EXE}>
            )
        endforeach()
    endif()
endmacro()

############################## Main Example ##################################
macro_example(
    EXE OPLExample
    FILE "opl-example.cpp"
    DEPS sonic-drivers-static
    LINKS ${LIB_SDL2main} sonic-drivers-static
    NONE
)

macro_example(
    EXE ADLExample
    FILE "adl-example.cpp"
    DEPS sonic-drivers-static
    LINKS ${LIB_SDL2main} sonic-drivers-static
    NONE
)
