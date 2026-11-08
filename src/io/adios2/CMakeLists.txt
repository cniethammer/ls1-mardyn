# ADIOS2 support: Adios2Reader is part of the core (input reader for restarts),
# Adios2Writer is a plugin.
if(ENABLE_ADIOS2)
    target_sources(MarDyn PRIVATE Adios2Reader.cpp)
    target_link_libraries(mardyn_settings INTERFACE adios2::adios2)
    target_compile_definitions(mardyn_settings INTERFACE ENABLE_ADIOS2)
endif(ENABLE_ADIOS2)

mardyn_add_plugin(Adios2Writer
    SOURCES  Adios2Writer.cpp
    HEADER   Adios2Writer.h
    REQUIRES ENABLE_ADIOS2
)

if(ENABLE_UNIT_TESTS)
    add_subdirectory(tests)
endif(ENABLE_UNIT_TESTS)
