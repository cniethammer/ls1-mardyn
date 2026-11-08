# not used in any example, but SpatialProfile uses its callback FixRegion::getMoleculesInRegion if available
mardyn_add_plugin(FixRegion
    SOURCES FixRegion.cpp
    HEADER  FixRegion.h
)
