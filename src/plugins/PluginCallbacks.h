#ifndef SRC_PLUGINS_PLUGINCALLBACKS_H_
#define SRC_PLUGINS_PLUGINCALLBACKS_H_

#include <map>
#include <string>

#include "utils/FunctionWrapper.h"

/** @brief Optional interface for plugins exchanging functions with other plugins.
 *
 * Plugins implementing this interface in addition to PluginBase can provide functions to other
 * plugins and use functions provided by them. After all plugins have been created, the simulation
 * first calls registerCallbacks() of all plugins implementing the interface, and then
 * accessAllCallbacks() of all of them. Therefore, a plugin can access callbacks of plugins listed
 * after it in the input.
 *
 * Callbacks are identified by name, e.g. "<plugin name>::<function name>". The map only exists
 * while the callbacks are exchanged, so plugins have to store the callbacks they want to use.
 */
class PluginCallbacks {
public:
	virtual ~PluginCallbacks() = default;

	/**
	 * Register callbacks to callbackMap.
	 * This allows to make functions of a plugin accessible to other plugins.
	 * New callbacks should be added to callbackMap.
	 * Example syntax:
	 * - register a function that returns a local value:
	 * \code
	 *   callbackMap["getMyLocalValue"] = [this] { return _myLocalValue; };
	 * \endcode
	 * - register a function that calls a local function and returns its return value:
	 * \code
	 *   callbackMap["callMyFunct"] = [this] { return myFunct(); };
	 * \endcode
	 * @param callbackMap Add callbacks to this map.
	 */
	virtual void registerCallbacks(std::map<std::string, FunctionWrapper>& callbackMap) {
		// Empty by default.
	}

	/**
	 * Save callbacks from the callbackMap locally.
	 * This allows a plugin to call functions from other plugins.
	 * Example syntax:
	 * - store a function that returns an unsigned long:
	 * \code
	 *   std::function<unsigned long(void)> myFunction;
	 *   myFunction = callbackMap.at("getSomeLocalValue").get<unsigned long>();
	 * \endcode
	 * - store a function that calls some function of another plugin with an input value (int):
	 * \code
	 *   std::function<void(int)> myFunction;
	 *   myFunction = callbackMap.at("doSth").get<void, int>();
	 * \endcode
	 * @param callbackMap Get callbacks from this map.
	 */
	virtual void accessAllCallbacks(const std::map<std::string, FunctionWrapper>& callbackMap) {
		// Empty by default.
	}
};

#endif  // SRC_PLUGINS_PLUGINCALLBACKS_H_
