#include "PluginLoader.h"

#include "utils/Logger.h"
#include "utils/xmlfileUnits.h"

long loadPlugins(PluginFactory<PluginBase>& factory, XMLfileUnits& xmlconfig, const std::string& nodepath,
                 std::list<PluginBase*>& plugins) {
	const auto numPluginsBefore = plugins.size();

	XMLfile::Query query = xmlconfig.query(nodepath);
	unsigned long numPlugins = query.card();

	Log::global_log->info() << "Number of plugins with path " << nodepath << ": " << numPlugins << std::endl;
	if (numPlugins < 1) {
		Log::global_log->warning() << "No plugins specified for path " << nodepath << "." << std::endl;
	}

	for (auto pluginIter = query.begin(); pluginIter; ++pluginIter) {
		auto pluginNode = xmlconfig.scopedChangeCurrentNode(pluginIter);
		std::string pluginname("");
		xmlconfig.getNodeValue("@name", pluginname);
		bool enabled = true;
		xmlconfig.getNodeValue("@enabled", enabled);
		if (not enabled) {
			Log::global_log->debug() << "skipping disabled plugin: " << pluginname << std::endl;
			continue;
		}
		Log::global_log->info() << "Enabling plugin: " << pluginname << std::endl;

		PluginBase* plugin = factory.create(pluginname);

		plugin->readXML(xmlconfig);
		plugins.push_back(plugin);
	}

	return static_cast<long>(plugins.size() - numPluginsBefore);
}
