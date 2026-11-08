#include "DefaultPluginFactory.h"
#include "plugins/RegisteredPlugins.h"  // generated, see cmake/plugins.cmake

#include "utils/Logger.h"

// Output plugins
#include "io/ODF.h"
#include "io/RDF.h"

// General plugins
#include "plugins/Mirror.h"
#include "plugins/NEMD/DistControl.h"
#include "plugins/VectorizationTuner.h"

DefaultPluginFactory::DefaultPluginFactory() {
	Log::global_log->debug() << "REGISTERING PLUGINS" << std::endl;

	// plugins added with mardyn_add_plugin() in CMake
	registerBuiltinPlugins(*this);

	// plugins not yet moved to their own directories

	REGISTER_PLUGIN(Mirror);
	REGISTER_PLUGIN(ODF);
	REGISTER_PLUGIN(RDF);
	REGISTER_PLUGIN(DistControl);
	REGISTER_PLUGIN(VectorizationTuner);
}
