/*
 * BoundaryUtils.h
 *
 *  Created on: 24 March 2023
 *      Author: amartyads
 */

#pragma once

#include <map>
#include <vector>
#include <string>
#include "BoundaryType.h"
#include "DimensionType.h"

class BoundaryUtils
{
public:
	BoundaryUtils();
	BoundaryType getBoundary(std::string dimension) const;
	void setBoundary(std::string dimension, BoundaryType value);
	BoundaryType getBoundary(DimensionType dimension) const;
	void setBoundary(DimensionType dimension, BoundaryType value);
	DimensionType convertStringToDimension(std::string dimension) const;

private:
	std::map<DimensionType, BoundaryType> boundaries;
	const std::vector<std::string> permissibleDimensions = {"+x", "-x", "+y", "-y", "+z", "-z"};
};