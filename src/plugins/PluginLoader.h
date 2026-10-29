#ifndef SRC_PLUGINS_PLUGINLOADER_H_
#define SRC_PLUGINS_PLUGINLOADER_H_

#include <list>
#include <string>

#include "PluginBase.h"
#include "PluginFactory.h"

class XMLfileUnits;

/** @brief Create, configure and collect all plugins listed in an XML section.
 *
 * For every node matching @p nodepath, the plugin given by the attribute @c name is created
 * by @p factory, its readXML() is called with the node, and it is appended to @p plugins.
 * Nodes with attribute @c enabled="false" are skipped.
 * The current node of @p xmlconfig is restored on return, also if an exception is thrown.
 *
 * \code{.xml}
   <plugin name="RDF" enabled="yes"> ... </plugin>
   \endcode
 *
 * @param factory    Factory used to create the plugins.
 * @param xmlconfig  XML configuration, queried relative to its current node.
 * @param nodepath   Node path (relative to the current node of @p xmlconfig) selecting
 *                   the plugin nodes, e.g. "plugin" or "output/outputplugin".
 * @param plugins    List the newly created plugins are appended to; takes ownership.
 * @return Number of plugins loaded, i.e. appended to @p plugins; disabled plugins are not counted.
 * @throws PluginNotFoundError if an enabled node names a plugin that is not registered.
 */
long loadPlugins(PluginFactory<PluginBase>& factory, XMLfileUnits& xmlconfig, const std::string& nodepath,
                 std::list<PluginBase*>& plugins);

#endif  // SRC_PLUGINS_PLUGINLOADER_H_
