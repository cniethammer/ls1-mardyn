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
 * plugins and use functions provided by them. After all plugins have been created from the input,
 * the simulation first calls registerCallbacks() of all plugins implementing the interface, and then
 * accessAllCallbacks() of all of them. Therefore, a plugin can get callbacks of plugins listed
 * after it in the input.
 *
 * Callbacks are identified by name, following the pattern "<plugin name>::<function name>".
 *
 * Both functions are called before PluginBase::init() of any plugin, so the providing plugin may
 * not be initialized yet. Call a callback at the earliest in a function called after all plugins
 * have been initialized, e.g. PluginBase::endStep(), not in accessAllCallbacks() or
 * PluginBase::init().
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
	 * Get the callbacks of other plugins this plugin uses.
	 * The map only exists during this call, so store the callbacks, e.g. as std::function members,
	 * and call them later (see the class description). Use findCallback() for optional and
	 * requireCallback() for required callbacks:
	 * \code
	 *   void MyPlugin::accessAllCallbacks(const std::map<std::string, FunctionWrapper>& callbackMap) {
	 *       _doSth = requireCallback<void, int>(callbackMap, "OtherPlugin::doSth", getPluginName());
	 *   }
	 * \endcode
	 * @param callbackMap Callbacks registered by all plugins.
	 */
	virtual void accessAllCallbacks(const std::map<std::string, FunctionWrapper>& callbackMap) {
		// Empty by default.
	}

protected:
	/**
	 * Get an optional callback in accessAllCallbacks().
	 * \code
	 *   if (auto getValue = findCallback<unsigned long>(callbackMap, "OtherPlugin::getValue")) {
	 *       _getValue = *getValue;
	 *   }
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
	 * Get a callback the plugin cannot work without in accessAllCallbacks().
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
