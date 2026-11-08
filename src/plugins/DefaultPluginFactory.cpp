#include "DefaultPluginFactory.h"
#include "plugins/RegisteredPlugins.h"  // generated, see cmake/plugins.cmake

#include "utils/Logger.h"

// Output plugins
#include "io/ODF.h"
#include "io/RDF.h"

// General plugins
#include "plugins/COMaligner.h"
#include "plugins/DirectedPM.h"
#include "plugins/Dropaccelerator.h"
#include "plugins/Dropaligner.h"
#include "plugins/FixRegion.h"
#include "plugins/InMemoryCheckpointing.h"
#include "plugins/LoadImbalanceThroughSleepPlugin.h"
#include "plugins/MaxCheck.h"
#include "plugins/Mirror.h"
#include "plugins/MirrorSystem.h"
#include "plugins/NEMD/DistControl.h"
#include "plugins/Permittivity.h"
#include "plugins/SpatialProfile.h"
#include "plugins/VectorizationTuner.h"
#include "plugins/WallPotential.h"
#ifdef MAMICO_COUPLING
#include "plugins/MamicoCoupling.h"
#endif

DefaultPluginFactory::DefaultPluginFactory() {
	Log::global_log->debug() << "REGISTERING PLUGINS" << std::endl;

	// plugins added with mardyn_add_plugin() in CMake
	registerBuiltinPlugins(*this);

	// plugins not yet moved to their own directories

	REGISTER_PLUGIN(COMaligner);
	REGISTER_PLUGIN(DirectedPM);
	REGISTER_PLUGIN(Dropaccelerator);
	REGISTER_PLUGIN(Dropaligner);
	REGISTER_PLUGIN(FixRegion);
	REGISTER_PLUGIN(InMemoryCheckpointing);
	REGISTER_PLUGIN(SpatialProfile);
	REGISTER_PLUGIN(LoadImbalanceThroughSleepPlugin);
	REGISTER_PLUGIN(MaxCheck);
	REGISTER_PLUGIN(Mirror);
	REGISTER_PLUGIN(MirrorSystem);
	REGISTER_PLUGIN(ODF);
	REGISTER_PLUGIN(Permittivity);
	REGISTER_PLUGIN(RDF);
	REGISTER_PLUGIN(DistControl);
	REGISTER_PLUGIN(VectorizationTuner);
	REGISTER_PLUGIN(WallPotential);

#ifdef MAMICO_COUPLING
	REGISTER_PLUGIN(MamicoCoupling);
#endif
}
