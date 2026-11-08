target_sources(MarDyn
    PRIVATE
        ASCIIReader.cpp
        BinaryReader.cpp
        CubicGridGeneratorInternal.cpp
        IOHelpers.cpp
        MemoryProfiler.cpp
        Mkesfera.cpp
        MultiObjectGenerator.cpp
        ObjectGenerator.cpp
        ODF.cpp
        PerCellGenerator.cpp
        RDF.cpp
        ReplicaGenerator.cpp
        TaskTimingProfiler.cpp
        TcTS.cpp
        TimerProfiler.cpp
    )

if(ENABLE_UNIT_TESTS)
    add_subdirectory(tests)
endif(ENABLE_UNIT_TESTS)

# plugins and plugin groups in subdirectories (e.g. adios2/, vtk/), see cmake/plugins.cmake
mardyn_add_plugin_subdirectories()

