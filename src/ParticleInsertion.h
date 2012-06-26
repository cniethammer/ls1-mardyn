#ifndef _MOLECULARDYNAMICS_COUPLING_PARTICLEINSERTION_H_
#define _MOLECULARDYNAMICS_COUPLING_PARTICLEINSERTION_H_

#include "particleContainer/LinkedCells.h"
#include "tarch/la/Vector.h"
#include "molecules/Molecule.h"
#include <vector>
#include "MoleculeWrapper.h"
#include "MardynMoleculeWrapper.h"
#include <cstdlib>
#include <cmath>
#include <string>
using namespace std;

#define PI 3.1415926535

namespace moleculardynamics {
namespace coupling {
template<class Molecule, class LinkedCells, unsigned int dim>
class ParticleInsertion;
}
}

class Simulation;
class ParticleInsertionConfiguration;

/** handles particle insertion (via Usher algorithm) and random particle deletion.
 *
 *  @author Philipp Neumann
 */
template<class Molecule, class LinkedCells, unsigned int dim>
class moleculardynamics::coupling::ParticleInsertion {
public:
	ParticleInsertion();
	~ParticleInsertion() {
	}

	/** this state is returned by the insertDeleteMass() function and tells the user, if mass was inserted/deleted
	 *  or if nothing happened at all.
	 */
	enum Action {
		NoAction = 0, Insertion = 1, Deletion = 2
	};

	/** inserts a particle in the respective macroscopic cell. Returns Insertion on success and NoAction otherwise. */
	/*
	 typename moleculardynamics::coupling::ParticleInsertion<Molecule,LinkedCells,dim>::Action insertParticle(
	 //moleculardynamics::coupling::MacroscopicCell<LinkedCells,dim>& cell,
	 const tarch::la::Vector<dim,unsigned int>& cellIndex,
	 const tarch::la::Vector<dim,double>& macroscopicCellPosition,
	 const tarch::la::Vector<dim,double>& macroscopicCellSize,
	 const tarch::la::Vector<dim,double>& meanVelocity,
	 const double &temperature
	 ) const;
	 */

	/** deletes a particle from the macroscopic cell. Returns Deletion on success and NoAction otherwise. */
	/*
	 typename moleculardynamics::coupling::ParticleInsertion<Molecule,LinkedCells,dim>::Action deleteParticle(
	 moleculardynamics::coupling::MacroscopicCell<LinkedCells,dim>& cell
	 ) const;
	 */

	/** determines the position of a new particle within the macroscopic cell thisCell and stores the result
	 *  in the position entry of the molecule 'molecule'. The method returns Insertion if a position has been
	 *  found and NoAction otherwise.
	 */
	Action findParticlePosition(
			LinkedCells* linkedCells,
			ParticleCell thisCell,
			const tarch::la::Vector<dim,unsigned int>& cellIndex,
			const tarch::la::Vector<dim,double>& macroscopicCellPosition,
			const tarch::la::Vector<dim,double>& macroscopicCellSize,
			Molecule* molecule,
			Simulation* simulation,
			double U_0, double* energy, double* old_energy,
			bool largeStepsizeOnOverlap, bool restartIfIncreases, int seed,
			int intIterMax, int restartMax,
			int rotationsMax, double maxAngle, double maxAllowedAngle, double minAngle, double xiMax,
			vector<double>* vec_energy, vector<double>* vec_angle,
			vector<double*>* vec_lj, vector<double*>* vec_center,
			string name_energy, string name_angle, string name_lj, string name_center) const;

	int rotateMolecule(Molecule* molecule,
			moleculardynamics::coupling::interface::MardynMoleculeWrapper<Molecule,
			dim> wrapper, LinkedCells* linkedCells, Simulation* simulation,
			int rotationsMax, double maxAngle, double maxAllowedAngle, double minAngle,
			double U_0, double U_overlap, int* timestep,
			double xiMax, double* energy, double* energyOld,
			tarch::la::Vector<dim, double>* force_vec, double* absForce,
			Quaternion* q, bool stopIfOverlapping, bool log,
			vector<double>* vec_energy, vector<double>* vec_angle,
			vector<double*>* vec_lj, vector<double*>* vec_center,
			string name_energy, string name_angle, string name_lj, string name_center) const;
	void writeMoleculeVtk(Molecule* molecule, LinkedCells* linkedCells,
			Simulation* simulation, int* timestep) const;

	void doLogging(vector<double>* vec_energy, vector<double>* vec_angle,
			vector<double*>* vec_lj, vector<double*>* vec_center,
			string name_energy, string name_angle, string name_lj,
			string name_center) const;

	void writeToVectors(vector<double>* vec_energy, vector<double>* vec_angle,
			vector<double*>* vec_lj, vector<double*>* vec_center, double energy, double angle,
			Molecule* molecule) const;

	void write_vector(vector<double> vec, string file_name) const;
	void write_vector_3d(vector<double*> vec, string file_name) const;

	// const moleculardynamics::coupling::configurations::ParticleInsertionConfiguration _config;
	// tarch::logging::Log _log;
};
#include "ParticleInsertion.cpph"
#endif // _MOLECULARDYNAMICS_COUPLING_PARTICLEINSERTION_H_
