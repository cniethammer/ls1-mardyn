# used by almost all configurations, so it is always enabled by default
mardyn_add_plugin(CheckpointWriter
    SOURCES  CheckpointWriter.cpp
    HEADER   CheckpointWriter.h
    DEFAULT  ON
)
