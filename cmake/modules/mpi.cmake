# mpi
option(ENABLE_MPI "Enable MPI" OFF)
if(ENABLE_MPI)
    message(STATUS "MPI Enabled")
    find_package(MPI REQUIRED)
    set(MPI_LIB "MPI::MPI_CXX")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_MPI")

    option(ENABLE_PERSISTENT "Enable MPI Persistent Collective Communication" OFF)
    if(ENABLE_PERSISTENT)
        message(STATUS "MPI Persistent Collective Communication Enabled")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_PERSISTENT")
    else()
        message(STATUS "MPI Persistent Collective Communication Disabled")
    endif()
else()
    message(STATUS "MPI Disabled")
    set(MPI_LIB "")
endif()
