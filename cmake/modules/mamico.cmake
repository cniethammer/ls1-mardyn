option(MAMICO_COUPLING "Couple with MaMiCo" OFF)
if (MAMICO_COUPLING)
	message(STATUS "MaMiCo coupling enabled. mardyn will compile as library. No executable will be created.")
	set(MAMICO_COMPILE_DEFINITION "MAMICO_COUPLING")
    if(NOT MAMICO_SRC_DIR)
		message(FATAL_ERROR "MaMiCo source directory not specified.")
	endif()
else()
    message(STATUS "MaMiCo coupling disabled.")
endif()