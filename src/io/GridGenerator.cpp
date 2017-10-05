#include "io/GridGenerator.h"

#include "Domain.h"
#include "Simulation.h"
#include "ensemble/EnsembleBase.h"
#include "molecules/Molecule.h"
#include "particleContainer/ParticleContainer.h"
#include "parallel/DomainDecompBase.h"
#include "utils/Logger.h"
#include "utils/xmlfileUnits.h"
#include "molecules/MoleculeIdPool.h"
#include "utils/generator/Generator.h"

#include <cmath>
#include <limits>
#include <map>
#include <random>
#include <string>

#ifdef ENABLE_MPI
#include <mpi.h>
#endif

using Log::global_log;
using namespace std;

/** The VelocityAssignerBase implements the gernal functionality and interface to assign velocity vectors mathing to a given temperature.
 */
class VelocityAssignerBase {
public:
	VelocityAssignerBase(double T = 0) : _T(T) {}
	~VelocityAssignerBase(){}
	void setTemperature(double T) { _T = T; }
	double T() { return _T; }
	virtual void assignVelocity(Molecule *molecule) = 0;
private:
	double _T;  //!< coressponding target temperature
};


/** The VelocityAssigner can be used to assign normally distributed velocity vectors with absolute value mathching to a given temperature.
 */
class EqualVelocityAssigner : public VelocityAssignerBase {
public:
	EqualVelocityAssigner(double T = 0, int seed = 0) : VelocityAssignerBase(T), _mt(seed), _uniformDistrubtion(0, 1) {}
	~EqualVelocityAssigner(){}

	void assignVelocity(Molecule *molecule) {
		double v_abs = sqrt(/*kB=1*/ T() / molecule->component()->m());
		/* pick angels for uniform distributino on S^2. */
		double phi, theta;
		phi   = 2*M_PI * _uniformDistrubtion(_mt);
		theta = acos(2 * _uniformDistrubtion(_mt) - 1);
		double v[3];
		v[0] = v_abs * sin(phi);
		v[1] = v_abs * cos(phi) * sin(theta);
		v[2] = v_abs * cos(phi) * cos(theta);
		for(int d = 0; d < 3; d++) {
			molecule->setv(d, v[d]);
		}
	}
private:
	std::mt19937 _mt; //!< Mersenne twister used as input for the uniform distribution
	std::uniform_real_distribution<double> _uniformDistrubtion;
};


/** The MaxwellVelocityAssigner can be used to assign maxwell boltzmann distributed velocity vectors mathching to a given temperature.
 */
class MaxwellVelocityAssigner : public VelocityAssignerBase {
public:
	MaxwellVelocityAssigner(double T = 0, int seed = 0) : VelocityAssignerBase(T), _mt(seed), _normalDistrubtion(0.0, 1.0) {}
	~MaxwellVelocityAssigner() {}

	void assignVelocity(Molecule *molecule) {
		double v_abs = sqrt(/*kB=1*/ T() / molecule->component()->m());
		double v[3];
		v[0] = v_abs * _normalDistrubtion(_mt);
		v[1] = v_abs * _normalDistrubtion(_mt);
		v[2] = v_abs * _normalDistrubtion(_mt);
		for(int d = 0; d < 3; d++) {
			molecule->setv(d, v[d]);
		}
	}
private:
	std::mt19937 _mt; //!< Mersenne twister used as input for the normal distribution
	std::normal_distribution<double> _normalDistrubtion;
};


void GridGenerator::readXML(XMLfileUnits& xmlconfig) {
	XMLfile::Query query = xmlconfig.query("subgenerator");
	global_log->info() << "Number of sub-generators: " << query.card() << endl;
	string oldpath = xmlconfig.getcurrentnodepath();
	for( auto generatorIter = query.begin(); generatorIter; ++generatorIter ) {
		xmlconfig.changecurrentnode(generatorIter);
		_generators.push_back(new Generator);
		_generators.back()->readXML(xmlconfig);
	}
	xmlconfig.changecurrentnode(oldpath);
	std::string velocityAssignerName;
	xmlconfig.getNodeValue("velocityAssigner", velocityAssignerName);
	if(velocityAssignerName == "EqualVelocityDistribution") {
		_velocityAssigner = new EqualVelocityAssigner();
	} else if(velocityAssignerName == "MaxwellVelocityDistribution") {
		_velocityAssigner = new MaxwellVelocityAssigner();
	}
}


long unsigned int GridGenerator::readPhaseSpace(ParticleContainer* particleContainer, list<ChemicalPotential>* lmu,
		Domain* domain, DomainDecompBase* domainDecomp) {
	unsigned long numMolecules = 0;

	Ensemble* ensemble = _simulation.getEnsemble();
	double bBoxMin[3];
	double bBoxMax[3];
	domainDecomp->getBoundingBoxMinMax(domain, bBoxMin, bBoxMax);
	MoleculeIdPool moleculeIdPool(std::numeric_limits<unsigned long>::max(), domainDecomp->getNumProcs(), domainDecomp->getRank());

	_velocityAssigner->setTemperature(ensemble->T());
	for(auto generator : _generators) {
		Molecule molecule;
		generator->setBoudingBox(bBoxMin, bBoxMax);
		generator->init();
		while(generator->getMolecule(&molecule) > 0) {
			molecule.setid(moleculeIdPool.getNewMoleculeId());
			_velocityAssigner->assignVelocity(&molecule);
			Quaternion q(1.0, 0., 0., 0.); /* orientation of molecules has to be set to a value other than 0,0,0,0! */
			molecule.setq(q);
			bool inserted = particleContainer->addParticle(molecule);
			if(inserted){
				numMolecules++;
			}
		}
	}
	global_log->debug() << "Number of locally inserted molecules: " << numMolecules << endl;
	particleContainer->updateMoleculeCaches();
	unsigned long globalNumMolecules = numMolecules;
#ifdef ENABLE_MPI
	MPI_Allreduce(MPI_IN_PLACE, &globalNumMolecules, 1, MPI_UNSIGNED_LONG, MPI_SUM, domainDecomp->getCommunicator());
#endif
	global_log->info() << "Number of inserted molecules: " << numMolecules << endl;
	//! @todo Get rid of the domain class calls at this place here...
	domain->setGlobalTemperature(ensemble->T());
	domain->setglobalNumMolecules(globalNumMolecules);
	domain->setglobalRho(numMolecules / ensemble->V() );
	return numMolecules;
}
