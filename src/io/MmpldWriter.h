#ifndef MMPLDWRITER_H_
#define MMPLDWRITER_H_

#include <string>
#include <vector>
#include <array>
#include <cstdint>

#include "plugins/PluginBase.h"
#include "molecules/MoleculeForwardDeclaration.h"

#ifdef ENABLE_MPI
#include "utils/MPI_Info_object.h"
#endif

enum MMPLD_Vertex_type : std::uint8_t {
	MMPLD_VERTEX_NONE       = 0,
	MMPLD_VERTEX_FLOAT_XYZ  = 1,
	MMPLD_VERTEX_FLOAT_XYZR = 2,
	MMPLD_VERTEX_SHORT_XYZ  = 3,
};

enum MMPLD_Color_type : std::uint8_t {
	MMPLD_COLOR_NONE       = 0,
	MMPLD_COLOR_UINT8_RGB  = 1,
	MMPLD_COLOR_UINT8_RGBA = 2,
	MMPLD_COLOR_FLOAT_I    = 3,
	MMPLD_COLOR_FLOAT_RGB  = 4,
	MMPLD_COLOR_FLOAT_RGBA = 5,
};

/** @brief Output plugin to generate a MegaMol™ Particle List Data file (*.mmpld).
 *
 * Depending on the write frequency (for example: every timestep, or every 10th, 100th, 1000th ...) the number
 * of frames can be controlled. The *.mmpld file can be visualized by visualization software like MegaMol.
 *
 * The attribute @c type of the plugin node selects how molecules are represented:
 * - @c simple (default): one sphere per molecule, one sphere type per component.
 * - @c multi: one sphere per LJ site, one sphere type per site.
 */
class MmpldWriter : public PluginBase
{
public:
	/** @brief Representation of molecules as spheres in the output. */
	enum class SphereRepresentation : std::uint8_t {
		Simple,  //!< one sphere per molecule
		Multi,   //!< one sphere per LJ site
	};

	MmpldWriter();
	virtual ~MmpldWriter() {}

	void readXML(XMLfileUnits& xmlconfig);

	void init(ParticleContainer *particleContainer,
              DomainDecompBase *domainDecomp, Domain *domain);
	void endStep(
            ParticleContainer *particleContainer,
            DomainDecompBase *domainDecomp, Domain *domain,
            unsigned long simstep
    );
	void finish(ParticleContainer *particleContainer, DomainDecompBase *domainDecomp, Domain *domain) {}

	std::string getPluginName() {
		return std::string("MmpldWriter");
	}
	static PluginBase* createInstance() { return new MmpldWriter(); }

private:
	void SetNumSphereTypes();
	void CalcNumSpheresPerType(ParticleContainer* particleContainer, std::uint64_t* numSpheresPerType);
	bool GetSpherePos(float *spherePos, Molecule* mol, std::uint8_t& nSphereTypeIndex);
	void CalcNumSpheresPerTypeSimple(ParticleContainer* particleContainer, std::uint64_t* numSpheresPerType);
	bool GetSpherePosSimple(float *spherePos, Molecule* mol, std::uint8_t& nSphereTypeIndex);
	void CalcNumSpheresPerTypeMulti(ParticleContainer* particleContainer, std::uint64_t* numSpheresPerType);
	bool GetSpherePosMulti(float *spherePos, Molecule* mol, std::uint8_t& nSphereTypeIndex);

	std::string getOutputFilename();
	void MultiFileApproachReset(ParticleContainer* particleContainer,
			DomainDecompBase* domainDecomp, Domain* domain);
	void PrepareWriteControl();
	long get_data_frame_header_size();
	long get_seekTable_size();
	void writeSeekTableEntry(int id, std::uint64_t offset);
	long get_data_list_header_size();
	long get_particle_data_size();
	long get_data_list_size(std::uint64_t particle_count);
	void write_frame_header(std::uint32_t num_data_lists);
	void write_particle_list_header(std::uint64_t particle_count, int sphereId);
	void write_frame(ParticleContainer* particleContainer, DomainDecompBase* domainDecomp);

private:
	/** First time step to be recorded */
	std::uint64_t _startTimestep;
	/** time steps between two records*/
	std::uint64_t _writeFrequency;
	/** Max time step up to which shall be recorded */
	std::uint64_t _stopTimestep;
	long _writeBufferSize;
	std::string _outputPrefix;
	std::string _timestampString;
	std::uint32_t _frameCount;
	std::uint8_t  _numComponents;
	std::uint8_t  _numSitesTotal;
	std::uint8_t  _numSphereTypes;
	std::vector<std::uint64_t> _seekTable;
	std::vector<std::uint8_t> _numSitesPerComp;
	std::vector<std::uint8_t> _nCompSitesOffset;
	SphereRepresentation _sphereRepresentation;
	bool _bWriteControlPrepared;

	long _fileCount;
	std::uint32_t _numFramesPerFile;

	std::uint16_t _mmpldversion;
	std::uint32_t _numSeekEntries;
	MMPLD_Vertex_type _vertex_type;
	MMPLD_Color_type _color_type;
	std::vector<float> _global_radius;
	std::vector< std::array<std::uint8_t, 4> > _global_rgba;
	std::vector< std::array<float, 2> > _global_intensity_range;


#ifdef ENABLE_MPI
	MPI_File _mpifh;
	MPI_Info_object _mpiinfo;
#endif
};

#endif /* MMPLDWRITER_H_ */
