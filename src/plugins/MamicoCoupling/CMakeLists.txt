# Coupling with MaMiCo, which drives ls1 built as a library (see cmake/mamico.cmake).
mardyn_add_plugin(MamicoCoupling
    SOURCES  MamicoCoupling.cpp
    HEADER   MamicoCoupling.h
    REQUIRES MAMICO_COUPLING
)

# MaMiCo looks up the plugin by name, so the coupling does not work without it
if(MAMICO_COUPLING AND NOT MARDYN_PLUGIN_MamicoCoupling)
    message(FATAL_ERROR "MAMICO_COUPLING requires the plugin MamicoCoupling, enable it with "
                        "-DMARDYN_PLUGIN_MamicoCoupling=ON.")
endif()
