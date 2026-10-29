#ifndef SRC_PLUGINS_DEFAULTPLUGINFACTORY_H_
#define SRC_PLUGINS_DEFAULTPLUGINFACTORY_H_

#include <list>
#include <string>

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

	/** @brief Create, configure and collect all plugins listed in an XML section.
	 *
	 * For every node matching @p nodepath, the plugin given by the attribute @c name is created,
	 * its readXML() is called with the node, and it is appended to @p _plugins.
	 * Nodes with attribute @c enabled="false" are skipped.
	 * The current node of @p xmlconfig is restored on return.
	 *
	 * \code{.xml}
	   <plugin name="RDF" enabled="yes"> ... </plugin>
	   \endcode
	 *
	 * @param _plugins   list the newly created plugins are appended to; takes ownership.
	 * @param xmlconfig  XML configuration, queried relative to its current node.
	 * @param nodepath   Node path (relative to the current node of @p xmlconfig) selecting
	 *                   the plugin nodes, e.g. "plugin" or "output/outputplugin".
	 * @return Number of plugins enabled, i.e. appended to @p _plugins; disabled plugins are not counted.
	 */
	long enablePlugins(std::list<PluginBase*>& _plugins, XMLfileUnits& xmlconfig, const std::string& nodepath);
};

#endif  // SRC_PLUGINS_DEFAULTPLUGINFACTORY_H_
