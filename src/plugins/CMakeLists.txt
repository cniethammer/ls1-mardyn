target_sources(MarDyn
    PRIVATE
        DefaultPluginFactory.cpp
        MamicoCoupling.cpp
        Mirror.cpp
        PluginCallbacks.cpp
        PluginLoader.cpp
        VectorizationTuner.cpp
    )

if(ENABLE_UNIT_TESTS)
    add_subdirectory(tests)
endif(ENABLE_UNIT_TESTS)


# self-contained plugins in subdirectories, see cmake/plugins.cmake
mardyn_add_plugin_subdirectories()

