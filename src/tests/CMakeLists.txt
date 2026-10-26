option(ENABLE_UNIT_TESTS "Build unit tests" ON)
if(ENABLE_UNIT_TESTS)
    add_subdirectory(unit)
endif()

option(ENABLE_INTEGRATION_TESTS "Build integration tests" ON)
if(ENABLE_INTEGRATION_TESTS)
    add_subdirectory(integration)
endif()

# Make unit tests run before integration tests if both are enabled.
if(ENABLE_UNIT_TESTS AND ENABLE_INTEGRATION_TESTS)
    get_property(unit_tests DIRECTORY unit PROPERTY TESTS)
    get_property(integration_tests DIRECTORY integration PROPERTY TESTS)
    set_property(TEST ${integration_tests} APPEND PROPERTY DEPENDS ${unit_tests})
endif()

target_sources(MarDyn
    PRIVATE
        CommonTest.cpp
        MarDynInitOptionsTest.cpp
    )

