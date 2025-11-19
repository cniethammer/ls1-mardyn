target_sources(MarDyn
    PRIVATE
        CheckpointRestartTest.cpp
        MemoryProfilerTest.cpp
        RDFTest.cpp
    )

if(ENABLE_ADIOS2)
    target_sources(MarDyn PRIVATE Adios2IOTest.cpp)
endif(ENABLE_ADIOS2)
