target_sources(MarDyn
    PRIVATE
        COMaligner.cpp
        DefaultPluginFactory.cpp
        DirectedPM.cpp
        Dropaccelerator.cpp
        Dropaligner.cpp
        EnergyRAPL.cpp
        FixRegion.cpp
        InMemoryCheckpointing.cpp
        LoadImbalanceThroughSleepPlugin.cpp
        MamicoCoupling.cpp
        MaxCheck.cpp
        Mirror.cpp
        MirrorSystem.cpp
        Permittivity.cpp
        PluginLoader.cpp
        SpatialProfile.cpp
        VectorizationTuner.cpp
        WallPotential.cpp
    )

if(ENABLE_UNIT_TESTS)
    add_subdirectory(tests)
endif(ENABLE_UNIT_TESTS)


add_subdirectory(NEMD)
add_subdirectory(profiles)

# self-contained plugins in subdirectories, see cmake/plugins.cmake
mardyn_add_plugin_subdirectories()

