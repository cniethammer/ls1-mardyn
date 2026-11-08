# DistControl is used by core code (TemperatureControl, Planar, CuboidRegionObs) and by the Mirror
# plugin. It is therefore always built and registered in DefaultPluginFactory until it is decoupled.
target_sources(MarDyn
    PRIVATE
        DistControl.cpp
    )

# group option: the NEMD plugins below are only available (and shown in ccmake) if it is enabled
option(MARDYN_PLUGIN_NEMD "Build NEMD plugins" ON)

mardyn_add_plugin(DensityControl
    SOURCES  DensityControl.cpp
    HEADER   DensityControl.h
    REQUIRES MARDYN_PLUGIN_NEMD
)

mardyn_add_plugin(DriftCtrl
    SOURCES  DriftCtrl.cpp
    HEADER   DriftCtrl.h
    REQUIRES MARDYN_PLUGIN_NEMD
)

mardyn_add_plugin(ExtractPhase
    SOURCES  ExtractPhase.cpp
    HEADER   ExtractPhase.h
    REQUIRES MARDYN_PLUGIN_NEMD
)

mardyn_add_plugin(MettDeamon
    SOURCES  MettDeamon.cpp
    HEADER   MettDeamon.h
    REQUIRES MARDYN_PLUGIN_NEMD
)

mardyn_add_plugin(MettDeamonFeedrateDirector
    SOURCES  MettDeamonFeedrateDirector.cpp
    HEADER   MettDeamonFeedrateDirector.h
    REQUIRES MARDYN_PLUGIN_NEMD MARDYN_PLUGIN_MettDeamon
)

mardyn_add_plugin(PosNegComp
    SOURCES  PosNegComp.cpp
    HEADER   PosNegComp.h
    REQUIRES MARDYN_PLUGIN_NEMD
)

mardyn_add_plugin(RegionSampling
    SOURCES  RegionSampling.cpp
    HEADER   RegionSampling.h
    REQUIRES MARDYN_PLUGIN_NEMD
)

mardyn_add_plugin(VelocityExchange
    SOURCES  VelocityExchange.cpp
    HEADER   VelocityExchange.h
    REQUIRES MARDYN_PLUGIN_NEMD
)

if(ENABLE_UNIT_TESTS)
    add_subdirectory(tests)
endif(ENABLE_UNIT_TESTS)
