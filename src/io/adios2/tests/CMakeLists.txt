# round trip test: reads the checkpoint written with Adios2Writer using Adios2Reader
if(MARDYN_PLUGIN_Adios2Writer)
    target_sources(MarDyn PRIVATE Adios2IOTest.cpp)
endif()
