# Build settings shared by MarDyn and the plugin targets (see cmake/plugins.cmake).
# Everything that influences how headers are compiled (definitions, include directories,
# libraries) has to be set here, so that all objects linked into MarDyn agree.
add_library(mardyn_settings INTERFACE)
target_include_directories(mardyn_settings INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}"  # includes relative to src/
    "${CMAKE_CURRENT_BINARY_DIR}"  # generated headers, e.g. MarDyn_version.h
    )

if(MAMICO_COUPLING)
    add_library(MarDyn STATIC)
    target_include_directories(mardyn_settings SYSTEM INTERFACE "${MAMICO_SRC_DIR}")
    target_compile_definitions(mardyn_settings INTERFACE
        ${MAMICO_COMPILE_DEFINITIONS} ${MAMICO_MPI_DEFINITIONS}
        )
    if(MAMICO_ENABLE_FPIC)
        set_property(TARGET MarDyn PROPERTY POSITION_INDEPENDENT_CODE ON)
        set_property(TARGET mardyn_settings PROPERTY INTERFACE_POSITION_INDEPENDENT_CODE ON)
    endif()
else()
    add_executable(MarDyn)
endif()
target_link_libraries(MarDyn PUBLIC mardyn_settings)

target_sources(MarDyn
    PRIVATE
        Common.cpp
        Domain.cpp
        MarDyn.cpp
        Simulation.cpp
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

add_subdirectory(bhfmm)
add_subdirectory(ensemble)
add_subdirectory(integrators)
add_subdirectory(io)
add_subdirectory(longRange)
add_subdirectory(molecules)
add_subdirectory(particleContainer)
add_subdirectory(parallel)
add_subdirectory(plugins)
add_subdirectory(steereoCommands)
add_subdirectory(thermostats)
add_subdirectory(utils)

# registration code for plugins; has to come after all directories containing plugins
mardyn_generate_plugin_registration(plugins
    FUNCTION    registerBuiltinPlugins
    BASE        PluginBase
    BASE_HEADER plugins/PluginBase.h
    OUTPUT      plugins/RegisteredPlugins
    )


# we just add all libraries here. If a library is not set, it will simply be ignored.
target_link_libraries(mardyn_settings INTERFACE
        ${BLAS_LIB}    # for armadillo
        ${LAPACK_LIB}  # for armadillo
        ${VTK_LIB}     # for VTK/xerces
        ${CPPUNIT_LIB} # for unit tests
        ${AUTOPAS_LIB} # for autopas
        ${ALL_LIB}     # for ALL
        )

if(ENABLE_MPI)
    target_link_libraries(mardyn_settings INTERFACE MPI::MPI_CXX)
    target_compile_definitions(mardyn_settings INTERFACE ENABLE_MPI)
endif()

if(ENABLE_OPENMP)
    target_link_libraries(mardyn_settings INTERFACE OpenMP::OpenMP_CXX)
    target_compile_definitions(mardyn_settings INTERFACE ENABLE_OPENMP)
endif()

install(TARGETS MarDyn)
