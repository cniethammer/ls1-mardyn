#include "PluginCallbacks.h"

#include <sstream>
#include <stdexcept>

#if __has_include(<cxxabi.h>)
#include <cxxabi.h>

#include <cstdlib>
#include <memory>
#endif

namespace {

/** @brief Human readable name of a type, demangled if supported by the compiler. */
std::string typeName(const std::type_info& type) {
#if __has_include(<cxxabi.h>)
	int status = 0;
	std::unique_ptr<char, void (*)(void*)> demangled(abi::__cxa_demangle(type.name(), nullptr, nullptr, &status),
	                                                 std::free);
	if (status == 0 && demangled) {
		return demangled.get();
	}
#endif
	return type.name();
}

}  // namespace

void PluginCallbacks::throwSignatureMismatch(const std::string& name, const std::type_info& requested,
                                             const std::type_info& registered) {
	std::ostringstream message;
	message << "Callback '" << name << "' is registered as " << typeName(registered) << ", but requested as "
	        << typeName(requested) << ".";
	throw std::runtime_error(message.str());
}

void PluginCallbacks::throwMissing(const std::string& name, const std::string& requestingPlugin) {
	std::ostringstream message;
	message << "Plugin " << requestingPlugin << " requires callback '" << name << "', which is not registered.";
	// callback names follow the pattern "<plugin>::<function>"
	const auto separator = name.find("::");
	if (separator != std::string::npos) {
		message << " Is plugin " << name.substr(0, separator) << " enabled?";
	}
	throw std::runtime_error(message.str());
}
