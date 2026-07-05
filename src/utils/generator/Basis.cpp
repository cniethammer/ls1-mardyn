/*
 * Copyright (c) 2013      Christoph Niethammer <christoph.niethammer@gmail.com>
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER
 */

#include "utils/generator/Basis.h"
#include "utils/Logger.h"
#include "Simulation.h"
#include "ensemble/EnsembleBase.h"
#include "utils/Coordinate3D.h"
#include "utils/mardyn_assert.h"

#include <array>


void Basis::readXML(XMLfileUnits& xmlconfig) {
	XMLfile::Query query = xmlconfig.query("site");
	Ensemble* ensemble = _simulation.getEnsemble();
	std::string oldpath = xmlconfig.getcurrentnodepath();
	for(auto siteIter = query.begin(); siteIter; siteIter++) {
		Molecule molecule;
		xmlconfig.changecurrentnode(siteIter);
		int componentid = -1;
		std::string componentname = "";
		if(1 == xmlconfig.getNodeValue("componentname", componentname)) {
			molecule.setComponent(ensemble->getComponent(componentname));
		}
		else if(1 ==	xmlconfig.getNodeValue("componentid", componentid)) {
			molecule.setComponent(ensemble->getComponent(componentid));
		}
		else {
			Log::global_log->error() << "Missing component specification for site" << std::endl;
			MARDYN_EXIT("Missing component specification");
		}

		Coordinate3D sitePosition(xmlconfig, "coordinate");
		molecule.setr(0, sitePosition.x());
		molecule.setr(1, sitePosition.y());
		molecule.setr(2, sitePosition.z());
		Quaternion q(1.0, 0., 0., 0.); /* orientation of molecules has to be set to a value other than 0,0,0,0! */
		molecule.setq(q);

		Log::global_log->info() << "[Basis] Adding molecule " << molecule.component()->getName() << " (cid=" << molecule.componentid() << "), (x,y,z)=(" << molecule.r(0) << "," << molecule.r(1) << "," << molecule.r(2) << ")" << std::endl;
		addMolecule(molecule);
	}
	xmlconfig.changecurrentnode(oldpath);
}

void Basis::addMolecule(const Molecule& molecule) {
	_molecules.push_back(molecule);
}

Molecule Basis::getMolecule(size_t i) const {
	return _molecules.at(i);
}


size_t Basis::numMolecules() const {
	return _molecules.size();
}
