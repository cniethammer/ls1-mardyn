#ifndef SRC_IO_CHECKPOINTWRITER_H_
#define SRC_IO_CHECKPOINTWRITER_H_

#include <string>

#include "io/OutputBase.h"


class CheckpointWriter : public OutputBase {
public:
	
    CheckpointWriter() {}
	~CheckpointWriter() {}
	

	/** @brief Read in XML configuration for CheckpointWriter.
	 *
	 * The following xml object structure is handled by this method:
	 * \code{.xml}
	   <outputplugin name="CheckpointWriter">
	     <type>ASCII|binary</type>
	     <writefrequency>INTEGER</writefrequency>
	     <outputprefix>STRING</outputprefix>
	     <incremental>INTEGER</incremental>
	     <appendTimestamp>INTEGER</appendTimestamp>
	   </outputplugin>
	   \endcode
	 */
	void readXML(XMLfileUnits& xmlconfig);
	
	void initOutput(ParticleContainer* particleContainer,
			DomainDecompBase* domainDecomp, Domain* domain);
	void doOutput(
			ParticleContainer* particleContainer,
			DomainDecompBase* domainDecomp, Domain* domain,
			unsigned long simstep, std::list<ChemicalPotential>* lmu,
			std::map<unsigned, CavityEnsemble>* mcav
	);
	void finishOutput(ParticleContainer* particleContainer,
			DomainDecompBase* domainDecomp, Domain* domain);
	
	std::string getPluginName() {
		return std::string("CheckpointWriter");
	}
	static OutputBase* createInstance() { return new CheckpointWriter(); }
private:
	std::string _outputPrefix;
	unsigned long _writeFrequency;
    bool    _useBinaryFormat;
	bool	_incremental;
	bool	_appendTimestamp;
};

#endif  // SRC_IO_CHECKPOINTWRITER_H_