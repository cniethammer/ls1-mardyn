/*
 * VTKGridWriter.h
 *
 * @Date: 24.08.2010
 * @Author: eckhardw
 */

#pragma once

#ifndef MARDYN_AUTOPAS

#include "plugins/PluginBase.h"
#include "io/vtk/VTKGridCell.h"
#include "io/vtk/VTKGridVertex.h"

class LinkedCells;
class VTKGridWriterImplementation;

/**
 * This class acts as adapter to the VTKGridWriterImplementation, which handles
 * the actual xml writing. It is a friend class of LinkedCells, but reads only
 * its internal data to generate the vtk output.
 */
class VTKGridWriter : public PluginBase {

private:

	unsigned int _writeFrequency;

	std::string _fileName;

	VTKGridCell* _cells;

	VTKGridVertex* _vertices;

	//! length of the cells array
	int _numCells;

	//! length of the vertices array
	int _numVertices;

public:

	/**
	 * @param container the LinkedCells particle container. It has to be the same container
	 *                  which is handed in to the methods initOutput() / doOutput() / finishOutput()!
	 */
	VTKGridWriter();

	VTKGridWriter(unsigned int frequency, std::string name);

	virtual ~VTKGridWriter();

	virtual void init(ParticleContainer *particleContainer,
                      DomainDecompBase *domainDecomp, Domain *domain);

	/**
	 * creates the VTKGrid and sets all the data, which is then written out.
	 *
	 * @note As the gridwriter isn't notified when the particleContainer is changed
	 * (i.e. rebuild()), it is not sufficient to build up the vtkgrid once and only
	 * renew the information every iteration.
	 * Thus every time output is done, the methods setupVTKGrid() and releaseVTKGrid()
	 * are called.
	 */
	virtual void endStep(
            ParticleContainer *particleContainer, DomainDecompBase *domainDecomp,
            Domain *domain, unsigned long simstep
    );

	virtual void finish(ParticleContainer *particleContainer,
						DomainDecompBase *domainDecomp, Domain *domain);

	std::string getPluginName() {
		return std::string("VTKGridWriter");
	}
	static PluginBase* createInstance() { return new VTKGridWriter(); }

	void readXML(XMLfileUnits& xmlconfig);

private:

	void setupVTKGrid(ParticleContainer* particleContainer);

	void releaseVTKGrid();

	void getCellData(LinkedCells* container, VTKGridCell& cell);

	void outputParallelVTKFile(unsigned int numProcs, unsigned long simstep,
			VTKGridWriterImplementation& impl);

};

#endif /* MARDYN_AUTOPAS */
