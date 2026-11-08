mardyn_add_plugin(COMaligner
    SOURCES COMaligner.cpp
    HEADER  COMaligner.h
)

if(ENABLE_UNIT_TESTS)
    add_subdirectory(tests)
endif(ENABLE_UNIT_TESTS)
