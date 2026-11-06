# EnergyRAPL reads the RAPL energy counters and CPU topology from the Linux sysfs (/sys/...).
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(energy_rapl_default ON)
else()
    set(energy_rapl_default OFF)
endif()

mardyn_add_plugin(EnergyRAPL
    SOURCES EnergyRAPL.cpp
    HEADER  EnergyRAPL.h
    DEFAULT ${energy_rapl_default}
)

if(MARDYN_PLUGIN_EnergyRAPL AND NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "Plugin EnergyRAPL requires Linux as target system, but the target system is "
                        "'${CMAKE_SYSTEM_NAME}'. Disable it with -DMARDYN_PLUGIN_EnergyRAPL=OFF.")
endif()
