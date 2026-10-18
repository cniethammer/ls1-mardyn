option(ENABLE_OPENMP "Activates OpenMP shared memory parallelization." OFF)

if(ENABLE_OPENMP)
    find_package(OpenMP)
    message(STATUS "OpenMP enabled.")
else()
    message(STATUS "OpenMP disabled.")
endif()
