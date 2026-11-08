# used by almost all configurations, so it is always enabled by default
mardyn_add_plugin(ResultWriter
    SOURCES  ResultWriter.cpp
    HEADER   ResultWriter.h
    DEFAULT  ON
)
