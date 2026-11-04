#ifndef SRC_PLUGINS_PLUGINCALLBACKS_H_
#define SRC_PLUGINS_PLUGINCALLBACKS_H_

#include <any>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <typeinfo>

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

protected:
	/**
	 * Get an optional callback, e.g. in accessAllCallbacks().
	 * Example syntax:
	 * \code
	 *   std::optional<std::function<unsigned long()>> getValue = findCallback<unsigned long>(callbackMap,
	 * "Plugin::getValue"); if (getValue) { ... (*getValue)() ... }
	 * \endcode
	 * @tparam R     Return type of the callback.
	 * @tparam Args  Argument types of the callback.
	 * @param callbackMap  Map of all callbacks.
	 * @param name         Name of the callback.
	 * @return The callback, or std::nullopt if no callback with this name is registered.
	 * @throws std::runtime_error if the callback is registered with a different signature.
	 */
	template <class R, class... Args>
	static std::optional<std::function<R(Args...)>> findCallback(
		const std::map<std::string, FunctionWrapper>& callbackMap, const std::string& name) {
		const auto callback = callbackMap.find(name);
		if (callback == callbackMap.end()) {
			return std::nullopt;
		}
		try {
			return callback->second.template get<R, Args...>();
		} catch (const std::bad_any_cast&) {
			throwSignatureMismatch(name, typeid(std::function<R(Args...)>), callback->second.type());
		}
	}

	/**
	 * Get a callback the plugin cannot work without, e.g. in accessAllCallbacks().
	 * @tparam R     Return type of the callback.
	 * @tparam Args  Argument types of the callback.
	 * @param callbackMap       Map of all callbacks.
	 * @param name              Name of the callback.
	 * @param requestingPlugin  Name of the plugin requiring the callback, used in the error message.
	 * @return The callback.
	 * @throws std::runtime_error if no callback with this name is registered or it is registered with a
	 *         different signature.
	 */
	template <class R, class... Args>
	static std::function<R(Args...)> requireCallback(const std::map<std::string, FunctionWrapper>& callbackMap,
	                                                 const std::string& name, const std::string& requestingPlugin) {
		auto callback = findCallback<R, Args...>(callbackMap, name);
		if (not callback) {
			throwMissing(name, requestingPlugin);
		}
		return *callback;
	}

private:
	[[noreturn]] static void throwSignatureMismatch(const std::string& name, const std::type_info& requested,
	                                                const std::type_info& registered);
	[[noreturn]] static void throwMissing(const std::string& name, const std::string& requestingPlugin);
};

#endif  // SRC_PLUGINS_PLUGINCALLBACKS_H_
