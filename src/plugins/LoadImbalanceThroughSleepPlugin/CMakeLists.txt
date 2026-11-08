# test tool: artificially creates load imbalance to test load balancing, therefore disabled by default
mardyn_add_plugin(LoadImbalanceThroughSleepPlugin
    SOURCES LoadImbalanceThroughSleepPlugin.cpp
    HEADER  LoadImbalanceThroughSleepPlugin.h
    DEFAULT OFF
)
