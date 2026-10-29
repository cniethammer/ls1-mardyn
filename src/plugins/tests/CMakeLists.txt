target_sources(MarDyn
    PRIVATE
        COMalignerTest.cpp
        DensityControlTest.cpp
        VelocityExchangeTest.cpp
    )


# Catch2 based unit tests
find_package(Catch2 3 QUIET)
if(Catch2_FOUND)
    add_executable(PluginFactoryTest
        PluginFactoryTest.cpp
        ${PROJECT_SOURCE_DIR}/src/utils/Logger.cpp
        )
    target_include_directories(PluginFactoryTest PRIVATE ${PROJECT_SOURCE_DIR}/src)
    target_link_libraries(PluginFactoryTest PRIVATE Catch2::Catch2WithMain)

    include(Catch)
    catch_discover_tests(PluginFactoryTest TEST_PREFIX "PluginFactory.")
else()
    message(STATUS "Catch2 not found: PluginFactoryTest will not be built")
endif()
