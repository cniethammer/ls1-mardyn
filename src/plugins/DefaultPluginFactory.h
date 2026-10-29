#ifndef SRC_PLUGINS_DEFAULTPLUGINFACTORY_H_
#define SRC_PLUGINS_DEFAULTPLUGINFACTORY_H_

#include "PluginBase.h"
#include "PluginFactory.h"

/** @brief Factory for all simulation plugins (PluginBase) available in this build.
 *
 * The constructor registers all plugins; the set of registered plugins depends on
 * enabled CMake options (ADIOS2, VTK, MaMiCo, ...).
 */
class DefaultPluginFactory : public PluginFactory<PluginBase> {
public:
	DefaultPluginFactory();
};

#endif  // SRC_PLUGINS_DEFAULTPLUGINFACTORY_H_
