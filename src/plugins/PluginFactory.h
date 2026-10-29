#ifndef SRC_PLUGINS_PLUGINFACTORY_H_
#define SRC_PLUGINS_PLUGINFACTORY_H_

#include <map>
#include <string>
#include <vector>

#include "../utils/Logger.h"
#include "../utils/String_utils.h"



/** @brief Register plugin class NAME with the factory.
 *
 * Expands to registerPlugin(&NAME::createInstance); and can therefore only be used
 * inside member functions of PluginFactory or derived classes.
 * @param NAME  class name of the plugin; must provide a static createInstance() method.
 */
#define REGISTER_PLUGIN(NAME) registerPlugin(&(NAME::createInstance));


/** @brief Generic factory to create plugin objects by name.
 *
 * @tparam T  Common base class (interface) of the plugins managed by this factory.
 *
 * Plugins are identified by the string returned from their getPluginName() method,
 * so T must provide:
 * \code{.cpp}
 *   virtual std::string getPluginName() = 0; // return the name of the plugin
 * \endcode
 * Each plugin class additionally has to provide a static creator function:
 * \code{.cpp}
 *   static T* createInstance() { return new MyPlugin(); }
 * \endcode
 * Plugins are registered with the REGISTER_PLUGIN(NAME) macro in the constructor
 * of a derived factory (see DefaultPluginFactory, ObjectFactory).
 * Names must be unique; later registrations with an already used name are ignored.
 * Objects returned by create() are owned by the caller.
 */
template <typename T>
class PluginFactory {

typedef T* createInstanceFunc();

private:
	std::map<std::string, createInstanceFunc*> _pluginFactoryMap;

public:
	PluginFactory() {}
	~PluginFactory() {}


	/** @brief Register an output plugin
	 *
	 * @param createInstance  pointer to a function returning an instance of the plugin object.
	 */
	void registerPlugin(createInstanceFunc* createInstance) {
		T *pluginInstance = createInstance();
		std::string pluginname = pluginInstance->getPluginName();
		Log::global_log->debug() << "Registering plugin with name " << pluginname << std::endl;
		delete pluginInstance;
		if( _pluginFactoryMap.count(pluginname) > 0 ) {
			Log::global_log->warning() << "Skipping already registered plugin with name " << pluginname << std::endl;
			Log::global_log->debug() << "Registered plugins: " << string_utils::join(getPluginNames(), ", ") << std::endl;
			return;
		}
		_pluginFactoryMap[pluginname] = createInstance;
	}

	/** @brief Get all names of registered plugins
	 *
	 * @return Names of all registered plugins; empty if none are registered.
	*/
	std::vector<std::string> getPluginNames() {
		std::vector<std::string> pluginNames;
		for(auto const &plugin : _pluginFactoryMap) {
			pluginNames.push_back(plugin.first);
		}
		return pluginNames;
	}

	/** @brief Create a new instance of plugin
	 *
	 * @param pluginname Name of the plugin
	 * @return Newly allocated plugin instance, or nullptr if no plugin with this name is registered.
	*/
	T* create(std::string pluginname) {
		auto existing = _pluginFactoryMap.find(pluginname);
		if( existing != _pluginFactoryMap.end() ) {
			return existing->second(); /* call createInstance for plugin */
		}
		return nullptr;
	}
};
#endif  // SRC_PLUGINS_PLUGINFACTORY_H_
