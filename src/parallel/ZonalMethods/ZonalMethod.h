/*
 * ZonalMethod.hpp
 *
 *  Created on: Feb 28, 2017
 *      Author: seckler
 */

#pragma once

#include <vector>
#include <functional>
#include "../HaloRegion.h"

class ZonalMethod {
public:
	ZonalMethod();

	virtual ~ZonalMethod();

	/**
	 * Returns the export halo Regions of the process. These regions are also the ForceExport Regions of a process, if it is a force-exporter.
	 * This indicates, where the processes lie that require halo copies from the current process.
	 * @param initialRegion boundary of the current process
	 * @param cutoffRadius
	 * @return vector of regions
	 */
	virtual std::vector<HaloRegion> getHaloImportForceExportRegions(HaloRegion& initialRegion, double cutoffRadius,
			bool coversWholeDomain[3])=0;

	/**
	 * Returns the export leaving Regions of the process.
	 * This indicates, where the processes lie that get leaving particles from the current process.
	 * @param initialRegion boundary of the current process
	 * @param cutoffRadius
	 * @return vector of regions
	 */
	virtual std::vector<HaloRegion> getLeavingExportRegions(HaloRegion& initialRegion, double cutoffRadius,
			bool coversWholeDomain[3]);


protected:

	/**
	 * Returns the haloRegions outside of the initialRegion using an additional condition.
	 * Up to 26 neighbouring HaloRegions are constructed. Only if the domain does not cover the whole domain
	 * and if the condition is fulfilled the Region is constructed.
	 * @param initialRegion
	 * @param cutoffRadius
	 * @param coversWholeDomain
	 * @param condition should return true, if the HaloRegion shall be included in the return value. Its input argument is the array of offsets.
	 * @return vector of HaloRegions
	 */
	std::vector<HaloRegion> getHaloRegionsConditional(HaloRegion& initialRegion, double cutoffRadius,
			bool coversWholeDomain[3], const std::function<bool(const int[3])>& condition);

};

