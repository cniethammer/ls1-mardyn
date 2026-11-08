# tests are only built if the tested plugin is enabled
if(MARDYN_PLUGIN_DensityControl)
    target_sources(MarDyn PRIVATE DensityControlTest.cpp)
endif()

if(MARDYN_PLUGIN_VelocityExchange)
    target_sources(MarDyn PRIVATE VelocityExchangeTest.cpp)
endif()
