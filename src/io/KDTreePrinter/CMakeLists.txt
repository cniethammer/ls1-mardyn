# outdated: not used in any example and not tested, therefore disabled by default
# TODO: KDTreePrinter only works with the KDDecomposition and therefore with MPI; without MPI it
#       only prints a warning. Decide whether to add REQUIRES ENABLE_MPI, which would turn
#       configurations using it in non-MPI builds into errors (PluginNotFoundError).
mardyn_add_plugin(KDTreePrinter
    SOURCES KDTreePrinter.cpp
    HEADER  KDTreePrinter.h
    DEFAULT OFF
)
