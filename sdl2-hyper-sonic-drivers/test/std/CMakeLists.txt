cmake_minimum_required (VERSION 3.21)

find_package(GTest 1.11 CONFIG REQUIRED)
include(GoogleTest)
include(CMakeParseArguments)

macro(macro_test) # ${ARGN} link libraries
    
    set(prefix TEST)
    #set(flags DEFAULT)
    set(singleValues EXE)
    set(multiValues FILES FIXTURES LINKS_PRIVATE)
    
    cmake_parse_arguments(${prefix} "${flags}" "${singleValues}" "${multiValues}" ${ARGN})
    #message("EXE: ${TEST_EXE}")
    #message("FILES: ${TEST_FILES}")
    #message("FIXTURES: ${TEST_FIXTURES}")
    #message("LINKS_PRIVATE: ${TEST_LINKS_PRIVATE}")
    add_executable (${TEST_EXE} ${TEST_FILES})
    target_include_directories(${TEST_EXE} PUBLIC
        ${CMAKE_SOURCE_DIR}/sdl2-hyper-sonic-drivers/src
        ${CMAKE_SOURCE_DIR}/sdl2-hyper-sonic-drivers/test
    )
    #target_link_libraries(${TEST_EXE} PRIVATE ${TEST_LINKS})
    target_link_libraries(${TEST_EXE} PRIVATE GTest::gtest GTest::gmock)
    add_custom_command(TARGET ${TEST_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:${TEST_EXE}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:${TEST_EXE}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:${TEST_EXE}>
    )

    set(fixtures ${TEST_FIXTURES})
    list(LENGTH fixtures num_fixtures)
    unset(fixtures)
    if(num_fixtures EQUAL 0)
        message("${TEST_EXE}: No fixtures declared")
    else()
        message("${TEST_EXE} Fixtures: ${TEST_FIXTURES}")
        foreach(fixture_i ${TEST_FIXTURES})
            message("copy fixture ${fixture_i}")
            add_custom_command(TARGET ${TEST_EXE} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${CMAKE_CURRENT_SOURCE_DIR}/${fixture_i}
                        ${CMAKE_CURRENT_BINARY_DIR}/${fixture_i}
            )
        endforeach()
    endif()

    target_link_libraries(${TEST_EXE} PRIVATE ${TEST_LINKS_PRIVATE})

    #add_test(
    #    NAME ${TEST_EXE}
    #    COMMAND ${TEST_EXE} --gtest_output=xml:report.xml
    #)

    gtest_discover_tests(${TEST_EXE} PROPERTIES TEST_DISCOVERY_TIMEOUT 600 DISCOVERY_TIMEOUT 600)
endmacro()

macro_test(
    EXE TestEChannelGrouFormatter
    FILES "eChannelGroupFormatterTest.cpp"
)

macro_test(
    EXE TestIloggerFormatter
    FILES "ILoggerFormatterTest.cpp"
)

