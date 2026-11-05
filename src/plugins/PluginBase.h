#ifndef PLUGINBASE_H_
#define PLUGINBASE_H_

#include <string>

class ParticleContainer;
class DomainDecompBase;
class Domain;
class XMLfileUnits;

/** @todo Mark all parameters as const: output plugins should not modify the state of the simulation. */
/** @todo get rid of the domain parameter */

/** @brief Interface for plugins, i.e. classes that are called at fixed points of the simulation.
 *
 * Plugins extend the simulation without changing its core, e.g. to write thermodynamic values,
 * checkpoints or visualization data, to sample profiles, or to modify the system, e.g. by
 * applying additional forces or controlling regions. Plugins are selected by name in the input file:
 * \code{.xml}
   <plugin name="MyPlugin"> <!-- options of the plugin --> </plugin>
   \endcode
 * The same plugin may be listed several times; each entry creates a separate instance.
 *
 * The simulation calls the methods of all plugins in the order the plugins are listed in the input:
 * -# readXML() once directly after the plugin is created while reading the input. The phase space
 *    is not read at this point.
 * -# PluginCallbacks::registerCallbacks() and PluginCallbacks::accessAllCallbacks(), if the plugin
 *    also implements the optional PluginCallbacks interface.
 * -# init() once before the first time step, after the initial forces have been calculated.
 * -# afterForces() and endStep() once for the initial state before the first time step.
 * -# In each time step: beforeEventNewTimestep(), beforeForces(), siteWiseForces(), afterForces()
 *    and endStep().
 * -# finish() once after the last time step.
 *
 * A plugin has to implement readXML(), init(), endStep(), finish() and getPluginName(); the other
 * methods do nothing by default. In addition, each plugin class has to provide a static function
 * creating an instance, which is used by the PluginFactory to create plugins by name:
 * \code{.cpp}
 *   static PluginBase* createInstance() { return new MyPlugin(); }
 * \endcode
 * See plugins/ExamplePlugin for a complete example.
 */
class PluginBase {
public:
	PluginBase() = default;

	virtual ~PluginBase() = default;

	/** @brief Called once at the begin of the simulation, before the first time step.
	 *
	 * The phase space and the initial forces are available. Can be used e.g. to open output files
	 * or to initialize statistics.
	 * @param particleContainer  particle container storing the (local) molecules
	 * @param domainDecomp       domain decomposition in use
	 * @param domain
	 */
	virtual void init(ParticleContainer* particleContainer, DomainDecompBase* domainDecomp, Domain* domain) = 0;

	/** @brief Reads the configuration of the plugin from its section in the input file.
	 *
	 * Called once directly after the plugin has been created. The current node of @p xmlconfig is
	 * the plugin node, so options are queried relative to it:
	 * \code{.xml}
	   <plugin name="MyPlugin">
	     <!-- options of the plugin -->
	   </plugin>
	   \endcode
	 *
	 * @param xmlconfig  input file with the plugin node as current node
	 */
	virtual void readXML(XMLfileUnits& xmlconfig) = 0;

	/** @brief Called first thing in each time step.
	 * @param particleContainer  particle container storing the (local) molecules
	 * @param domainDecomp       domain decomposition in use
	 * @param simstep            current time step
	 */
	virtual void beforeEventNewTimestep(ParticleContainer* /* particleContainer */,
	                                    DomainDecompBase* /* domainDecomp */, unsigned long /* simstep */) {}

	/** @brief Called in each time step before the forces are calculated.
	 *
	 * Forces must not be modified here, as they are reset by the force calculation.
	 * @param particleContainer  particle container storing the (local) molecules
	 * @param domainDecomp       domain decomposition in use
	 * @param simstep            current time step
	 */
	virtual void beforeForces(ParticleContainer* /* particleContainer */, DomainDecompBase* /* domainDecomp */,
	                          unsigned long /* simstep */) {}

	/** @brief Called in each time step after the pair forces have been calculated, while the forces
	 * are still stored per site.
	 *
	 * Forces acting on single sites or on whole molecules can be added here.
	 * @param particleContainer  particle container storing the (local) molecules
	 * @param domainDecomp       domain decomposition in use
	 * @param simstep            current time step
	 */
	virtual void siteWiseForces(ParticleContainer* /* particleContainer */, DomainDecompBase* /* domainDecomp */,
	                            unsigned long /* simstep */) {}

	/** @brief Called after the forces have been calculated and summed up per molecule.
	 *
	 * Called in each time step and once for the initial state before the first time step. Site-wise
	 * forces can no longer be modified here.
	 * @param particleContainer  particle container storing the (local) molecules
	 * @param domainDecomp       domain decomposition in use
	 * @param simstep            current time step
	 */
	virtual void afterForces(ParticleContainer* /* particleContainer */, DomainDecompBase* /* domainDecomp */,
	                         unsigned long /* simstep */) {}

	/** @brief Called at the end of each time step.
	 *
	 * Also called once for the initial state before the first time step. Can be used e.g. to write
	 * data of the time step to a file or to perform additional computations.
	 * @param particleContainer  particle container storing the (local) molecules
	 * @param domainDecomp       domain decomposition in use
	 * @param domain
	 * @param simstep            current time step
	 */
	virtual void endStep(ParticleContainer* particleContainer, DomainDecompBase* domainDecomp, Domain* domain,
	                     unsigned long simstep) = 0;

	/** @brief Called once at the end of the simulation, after the last time step.
	 *
	 * Can be used e.g. to close output files or to write final statistics.
	 * @param particleContainer  particle container storing the (local) molecules
	 * @param domainDecomp       domain decomposition in use
	 * @param domain
	 */
	virtual void finish(ParticleContainer* particleContainer, DomainDecompBase* domainDecomp, Domain* domain) = 0;

	/** @brief Returns the name of the plugin, as used in the input file to select it. */
	virtual std::string getPluginName() = 0;
};

#endif /* PLUGINBASE_H_ */
