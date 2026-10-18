option(ENABLE_UNIT_TESTS "Build unit tests" ON)
if(ENABLE_UNIT_TESTS)
    add_subdirectory(unit)
endif()

option(ENABLE_INTEGRATION_TESTS "Build integration tests" ON)
if(ENABLE_INTEGRATION_TESTS)
    add_subdirectory(integration)
endif()

target_sources(MarDyn
    PRIVATE
        CommonTest.cpp
        MarDynInitOptionsTest.cpp
    )

