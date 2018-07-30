
#include "io/MaxWriter.h"

#include "Domain.h"
#include "parallel/DomainDecompBase.h"
#include "Simulation.h"
#include "utils/Logger.h"

#include <iomanip>
#include <vector>
#include <array>

using Log::global_log;
using namespace std;

MaxWriter::MaxWriter()
	:
	_writeFrequency(1000),
	_outputPrefix("maxvals"),
	_dMaxValuesLocal(nullptr),
	_dMaxValuesGlobal(nullptr),
	_numQuantities(7),
	_numValsPerQuantity(4),
	_numValsPerComponent(7*4),
	_numComponents(2),
	_numVals(7*4*2)
{
	_numComponents = global_simulation->getEnsemble()->getComponents()->size()+1;  // 0: all components
}

MaxWriter::~MaxWriter()
{
	delete[] _dMaxValuesLocal;
	delete[] _dMaxValuesGlobal;
}

void MaxWriter::readXML(XMLfileUnits& xmlconfig)
{
	global_log->info() << "------------------------------------------------------------------------" << std::endl;
	global_log->info() << "MaxWriter" << std::endl;

	_writeFrequency = 1000;
	xmlconfig.getNodeValue("writefrequency", _writeFrequency);
	global_log->info() << "Write frequency: " << _writeFrequency << endl;

	_outputPrefix = "maxvals";
	xmlconfig.getNodeValue("outputprefix", _outputPrefix);
	global_log->info() << "Output prefix: " << _outputPrefix << endl;

	global_log->info() << "------------------------------------------------------------------------" << std::endl;
}

void MaxWriter::initOutput(ParticleContainer* /*particleContainer*/,
			      DomainDecompBase* domainDecomp, Domain* /*domain*/)
{
	// init data structures
	this->initDataStructures();

	// initialize files (root process only)
	if( 0 != domainDecomp->getRank() )
		return;

	std::stringstream sstrFilename[4];
	sstrFilename[0] << _outputPrefix << "_max_veloc.dat";
	sstrFilename[1] << _outputPrefix << "_max_angmo.dat";
	sstrFilename[2] << _outputPrefix << "_max_force.dat";
	sstrFilename[3] << _outputPrefix << "_max_torqe.dat";

	std::stringstream sstrOutput[4];
	for(uint8_t qi=0; qi<_numQuantities; ++qi)
		sstrOutput[qi] << "                 simstep";

	for(uint32_t cid=0; cid<_numComponents; ++cid)
	{
		// velocity
		sstrOutput[0] << "                 vabs_c" << cid;
		sstrOutput[0] << "                  v+x_c" << cid;
		sstrOutput[0] << "                  v+y_c" << cid;
		sstrOutput[0] << "                  v+z_c" << cid;
		sstrOutput[0] << "                  v-x_c" << cid;
		sstrOutput[0] << "                  v-y_c" << cid;
		sstrOutput[0] << "                  v-z_c" << cid;

		// angular momentum
		sstrOutput[1] << "                 Labs_c" << cid;
		sstrOutput[1] << "                  L+x_c" << cid;
		sstrOutput[1] << "                  L+y_c" << cid;
		sstrOutput[1] << "                  L+z_c" << cid;
		sstrOutput[1] << "                  L-x_c" << cid;
		sstrOutput[1] << "                  L-y_c" << cid;
		sstrOutput[1] << "                  L-z_c" << cid;

		// force
		sstrOutput[2] << "                 Fabs_c" << cid;
		sstrOutput[2] << "                  F+x_c" << cid;
		sstrOutput[2] << "                  F+y_c" << cid;
		sstrOutput[2] << "                  F+z_c" << cid;
		sstrOutput[2] << "                  F-x_c" << cid;
		sstrOutput[2] << "                  F-y_c" << cid;
		sstrOutput[2] << "                  F-z_c" << cid;

		// torque
		sstrOutput[3] << "                 Mabs_c" << cid;
		sstrOutput[3] << "                  M+x_c" << cid;
		sstrOutput[3] << "                  M+y_c" << cid;
		sstrOutput[3] << "                  M+z_c" << cid;
		sstrOutput[3] << "                  M-x_c" << cid;
		sstrOutput[3] << "                  M-y_c" << cid;
		sstrOutput[3] << "                  M-z_c" << cid;
	}

	for(uint8_t qi=0; qi<_numQuantities; ++qi)
	{
		ofstream ofs(sstrFilename[qi].str().c_str(), ios::out);
		sstrOutput[qi] << endl;
		ofs << sstrOutput[qi].str();
		ofs.close();
	}
}

void MaxWriter::doOutput( ParticleContainer* particleContainer, DomainDecompBase* domainDecomp, Domain* domain,
	unsigned long simstep, list<ChemicalPotential>* /*lmu*/, map<unsigned, CavityEnsemble>* mcav )
{
	this->doSampling(particleContainer);

	if(simstep % _writeFrequency != 0)
		return;

	this->calculateGlobalValues();
	this->resetLocalValues();
	this->writeData(domainDecomp);

}

void MaxWriter::finishOutput(ParticleContainer* /*particleContainer*/,
				DomainDecompBase* /*domainDecomp*/, Domain* /*domain*/)
{
}

void MaxWriter::initDataStructures()
{
	_numQuantities = 4;  // velocity, angular momentum, force, torque
	_numValsPerQuantity = 7;  // Quantity (Q): Qabs, Qx_max, Qy_max, Qz_max, Qx_min, Qy_min, Qz_min
	_numValsPerComponent = _numQuantities * _numValsPerQuantity;
	_numVals = _numValsPerComponent * _numComponents;

	_dMaxValuesLocal  = new double[_numVals];
	_dMaxValuesGlobal = new double[_numVals];

	this->resetLocalValues();
}

void MaxWriter::doSampling(ParticleContainer* particleContainer)
{
	for (ParticleIterator pit = particleContainer->iteratorBegin();
			pit != particleContainer->iteratorEnd(); ++pit)
	{
		uint32_t cid = pit->componentid()+1;  // 0: all components
		uint32_t nOffsetComponent = cid*_numValsPerComponent;
		std::array<std::array<double,7>,4> arrQuantities;
		// squared absolute values
		arrQuantities.at(0).at(0) = pit->v2();
		arrQuantities.at(1).at(0) = pit->L2();
		arrQuantities.at(2).at(0) = pit->F2();
		arrQuantities.at(3).at(0) = pit->M2();
		// x-component
		arrQuantities.at(0).at(1) = pit->v(0);
		arrQuantities.at(1).at(1) = pit->D(0);
		arrQuantities.at(2).at(1) = pit->F(0);
		arrQuantities.at(3).at(1) = pit->M(0);
		// y-component
		arrQuantities.at(0).at(2) = pit->v(1);
		arrQuantities.at(1).at(2) = pit->D(1);
		arrQuantities.at(2).at(2) = pit->F(1);
		arrQuantities.at(3).at(2) = pit->M(1);
		// z-component
		arrQuantities.at(0).at(3) = pit->v(2);
		arrQuantities.at(1).at(3) = pit->D(2);
		arrQuantities.at(2).at(3) = pit->F(2);
		arrQuantities.at(3).at(3) = pit->M(2);

		for(uint32_t qi=0; qi<_numQuantities; ++qi)
		{
			uint32_t nOffsetQuantity = _numValsPerQuantity*qi;

			// all components
			double* ptrValueActual = &arrQuantities.at(qi).at(0);
			double* ptrValueStored = &_dMaxValuesLocal[nOffsetQuantity];
			if(*ptrValueActual > *ptrValueStored)
				*ptrValueStored = *ptrValueActual;
			for(uint32_t dim=1; dim<4; ++dim)
			{
				// positive direction (+)
				ptrValueActual = &arrQuantities.at(qi).at(dim);
				ptrValueStored = &_dMaxValuesLocal[nOffsetQuantity+dim];
				if(*ptrValueActual > *ptrValueStored)
					*ptrValueStored = *ptrValueActual;
				// negative direction (-)
				ptrValueActual = &arrQuantities.at(qi).at(dim);
				ptrValueStored = &_dMaxValuesLocal[nOffsetQuantity+dim+3];
				if(*ptrValueActual < *ptrValueStored)
					*ptrValueStored = *ptrValueActual;
			}
			// specific component
			ptrValueActual = &arrQuantities.at(qi).at(0);
			ptrValueStored = &_dMaxValuesLocal[nOffsetComponent+nOffsetQuantity];
			if(*ptrValueActual > *ptrValueStored)
				*ptrValueStored = *ptrValueActual;
			for(uint32_t dim=1; dim<4; ++dim)
			{
				// positive direction (+)
				ptrValueActual = &arrQuantities.at(qi).at(dim);
				ptrValueStored = &_dMaxValuesLocal[nOffsetComponent+nOffsetQuantity+dim];
				if(*ptrValueActual > *ptrValueStored)
					*ptrValueStored = *ptrValueActual;
				// negative direction (-)
				ptrValueActual = &arrQuantities.at(qi).at(dim);
				ptrValueStored = &_dMaxValuesLocal[nOffsetComponent+nOffsetQuantity+dim+3];
				if(*ptrValueActual < *ptrValueStored)
					*ptrValueStored = *ptrValueActual;
			}
		}
	}
}

void MaxWriter::calculateGlobalValues()
{
#ifdef ENABLE_MPI

	MPI_Reduce( _dMaxValuesLocal, _dMaxValuesGlobal, _numVals, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

#else
	// Scalar quantities
	for(uint32_t vi=0; vi<_numVals; ++vi)
		_dMaxValuesGlobal[vi] = _dMaxValuesLocal[vi];
#endif
}

void MaxWriter::resetLocalValues()
{
	for(uint32_t vi=0; vi<_numVals; ++vi)
		_dMaxValuesLocal[vi] = 0.0;
}

void MaxWriter::writeData(DomainDecompBase* domainDecomp)
{
	// only root process writes data to files
	if( 0 != domainDecomp->getRank() )
		return;

	std::stringstream sstrFilename[4];
	sstrFilename[0] << _outputPrefix << "_max_veloc.dat";
	sstrFilename[1] << _outputPrefix << "_max_angmo.dat";
	sstrFilename[2] << _outputPrefix << "_max_force.dat";
	sstrFilename[3] << _outputPrefix << "_max_torqe.dat";

	std::stringstream sstrOutput[4];

	// write data to streams
	for(uint32_t qi=0; qi<_numQuantities; ++qi)
	{
		uint32_t nOffsetQuantity = _numValsPerQuantity*qi;

		sstrOutput[qi] << std::setw(24) << global_simulation->getSimulationStep();
		for(uint32_t cid=0; cid<_numComponents; ++cid)
		{
			uint32_t nOffsetComponent = cid*_numValsPerComponent;
			double vmax = sqrt(_dMaxValuesGlobal[nOffsetComponent+nOffsetQuantity]);
			sstrOutput[qi] << FORMAT_SCI_MAX_DIGITS << vmax;
			for(uint8_t vi=1; vi<_numValsPerQuantity; ++vi)
				sstrOutput[qi] << FORMAT_SCI_MAX_DIGITS << _dMaxValuesGlobal[nOffsetComponent+nOffsetQuantity+vi];
		}
		sstrOutput[qi] << endl;
	}

	// write streams to files
	for(uint8_t qi=0; qi<_numQuantities; ++qi)
	{
		ofstream ofs(sstrFilename[qi].str().c_str(), ios::app);
		ofs << sstrOutput[qi].str();
		ofs.close();
	}
}





















