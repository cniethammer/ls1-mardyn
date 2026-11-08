#ifndef INPUTBASE_H_
#define INPUTBASE_H_

#include <string>
#include <list>

class ParticleContainer;

class DomainDecompBase;

class Domain;

class XMLfileUnits;

//! @brief interface for any kind of input class
//!
//! @todo more comment
class InputBase {
public:
	InputBase() = default;

	virtual ~InputBase() = default;

	/** @brief Read the header information, e.g. the box size and the simulation time.
	 *
	 * Called after readXML() while reading the configuration, i.e. before the particle container
	 * is set up, so readers can set information on the domain that is needed for that.
	 * The default implementation does nothing.
	 * @param domain  domain to set the header information on
	 */
	virtual void readPhaseSpaceHeader(Domain* /*domain*/) {}

	virtual void readXML(XMLfileUnits& /*xmlconfig*/) {}

	/** @brief read the actual phase space information
	 * @return number of read in molecules added to the particle containers of all processes
	 */
	virtual unsigned long
	readPhaseSpace(ParticleContainer* particleContainer, Domain* domain, DomainDecompBase* domainDecomp) = 0;

};

#endif /*INPUTBASE_H_*/
