if(ENABLE_VTK)
    target_sources(MarDyn PRIVATE VTKGridWriterImplementationTest.cpp)
endif(ENABLE_VTK)

# tests of plugins are only built if the tested plugin is enabled
if(MARDYN_PLUGIN_VTKMoleculeWriter)
    target_sources(MarDyn
        PRIVATE
            VTKMoleculeWriterImplementationTest.cpp
            VTKMoleculeWriterTest.cpp
        )
endif()

if(MARDYN_PLUGIN_VTKGridWriter)
    target_sources(MarDyn PRIVATE VTKGridWriterTest.cpp)
endif()
