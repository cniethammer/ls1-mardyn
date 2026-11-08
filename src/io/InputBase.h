#ifndef INPUTBASE_H_
#define INPUTBASE_H_

#include <string>

class ParticleContainer;

class DomainDecompBase;

class Domain;

class XMLfileUnits;

/** @brief Interface for all phase space sources, i.e. readers of phase space files and generators.
 *
 * Readers are created by name with a PluginFactory<InputBase> (see DefaultReaderFactory) and
 * selected in the XML configuration with
 * \code{.xml}
   <phasespacepoint>
     <file type="NAME">...</file>       <!-- or -->
     <generator name="NAME">...</generator>
   </phasespacepoint>
   \endcode
 * where NAME is the name returned by getPluginName().
 *
 * The simulation calls the methods in the following order:
 *  1. readXML() with the current node set to the file or generator node,
 *  2. readPhaseSpaceHeader(), while reading the configuration, i.e. before the domain
 *     decomposition and the particle container are set up,
 *  3. readPhaseSpace(), once the particle container is set up.
 *
 * Each reader additionally has to provide a static creator function for the factory:
 * \code{.cpp}
 *   static InputBase* createInstance() { return new MyReader(); }
 * \endcode
 */
class InputBase {
public:
	InputBase() = default;

	virtual ~InputBase() = default;

	InputBase(const InputBase&) = delete;
	InputBase& operator=(const InputBase&) = delete;

	/** @brief Name of the reader, as used in the XML configuration. */
	virtual std::string getPluginName() const = 0;

	/** @brief Read the configuration of the reader.
	 * @param xmlconfig  configuration, the current node is the file or generator node
	 */
	virtual void readXML(XMLfileUnits& /*xmlconfig*/) {}

	/** @brief Read the header information, e.g. the box size and the simulation time.
	 *
	 * Called before the particle container is set up, so readers can set information on the domain
	 * that is needed for that. The default implementation does nothing.
	 * @param domain  domain to set the header information on
	 */
	virtual void readPhaseSpaceHeader(Domain* /*domain*/) {}

	/** @brief Read or generate the actual phase space.
	 * @return number of read in molecules added to the particle containers of all processes
	 */
	virtual unsigned long
	readPhaseSpace(ParticleContainer* particleContainer, Domain* domain, DomainDecompBase* domainDecomp) = 0;

};

#endif /*INPUTBASE_H_*/
