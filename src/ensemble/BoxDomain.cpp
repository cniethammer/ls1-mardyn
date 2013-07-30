#include "BoxDomain.h"

#include "utils/xmlfileUnits.h"
#include "utils/Logger.h"

using namespace std;
using Log::global_log;

BoxDomain::BoxDomain() {
	for( int d = 0; d < 3; d++)
		_rmin[d] = 0;
}

void BoxDomain::readXML(XMLfileUnits& xmlconfig) {
	xmlconfig.getNodeValueReduced("lx",_rmax[0]);
	xmlconfig.getNodeValueReduced("ly",_rmax[1]);
	xmlconfig.getNodeValueReduced("lz",_rmax[2]);
	global_log->info() << "Box lower cornder (x,y,z): " << _rmin[0] << "," << _rmin[1] << "," << _rmin[2] << endl;
	global_log->info() << "Box upper corner  (x,y,z): " << _rmax[0] << "," << _rmax[1] << "," << _rmax[2] << endl;
}

double BoxDomain::V() {
	return _rmax[0] * _rmax[1] * _rmax[2];
}

double BoxDomain::length(int d) {
	/* optimized as lower corner is set to 0,0,0 */
	return _rmax[d];
}
