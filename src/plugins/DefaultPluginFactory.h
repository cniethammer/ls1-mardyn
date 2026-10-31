#ifndef SRC_PLUGINS_DEFAULTPLUGINFACTORY_H_
#define SRC_PLUGINS_DEFAULTPLUGINFACTORY_H_

#include "PluginBase.h"
#include "PluginFactory.h"

/** @brief Factory for all simulation plugins (PluginBase) available in this build.
 *
 * The constructor registers all plugins: those added with mardyn_add_plugin() in CMake
 * (see cmake/plugins.cmake and registerBuiltinPlugins()) and those still listed explicitly in
 * DefaultPluginFactory.cpp. The set of registered plugins depends on enabled CMake options
 * (MARDYN_PLUGIN_<name>, ADIOS2, VTK, MaMiCo, ...).
 */
class DefaultPluginFactory : public PluginFactory<PluginBase> {
public:
	DefaultPluginFactory();
};

#endif  // SRC_PLUGINS_DEFAULTPLUGINFACTORY_H_
