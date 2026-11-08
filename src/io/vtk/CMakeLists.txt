# VTK output. The generated VTK file bindings and the grid writer implementation are part of the
# core, as they are also used by KDNode. The VTKMoleculeWriterImplementation is only used by the
# VTKMoleculeWriter plugin.
if(ENABLE_VTK)
    target_sources(MarDyn
        PRIVATE
            vtk-custom-decimal.cpp
            vtk-punstructured.cpp
            vtk-unstructured.cpp
            VTKGridCell.cpp
            VTKGridVertex.cpp
            VTKGridWriterImplementation.cpp
        )
endif(ENABLE_VTK)

mardyn_add_plugin(VTKMoleculeWriter
    SOURCES  VTKMoleculeWriter.cpp
             VTKMoleculeWriterImplementation.cpp
    HEADER   VTKMoleculeWriter.h
    REQUIRES ENABLE_VTK
)

# VTKGridWriter accesses the cells of LinkedCells, which are not available with AutoPas
mardyn_add_plugin(VTKGridWriter
    SOURCES  VTKGridWriter.cpp
    HEADER   VTKGridWriter.h
    REQUIRES ENABLE_VTK "NOT ENABLE_AUTOPAS"
)

if(ENABLE_UNIT_TESTS)
    add_subdirectory(tests)
endif(ENABLE_UNIT_TESTS)
