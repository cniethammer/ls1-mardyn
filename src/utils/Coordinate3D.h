#ifndef SRC_UTILS_COORDINATE3D_H_
#define SRC_UTILS_COORDINATE3D_H_

#include <array>
#include <string>

#include "utils/xmlfileUnits.h"

/** @brief The Coordinate3D class eases the handling of x y z coordinates in xml input.
 */
class Coordinate3D {
public:
	Coordinate3D() : _vec{0.0, 0.0, 0.0} {}
	/** @brief constructor directly reading in xml structure
	 * @param[in] xmlconfig  xml config file object
	 * @param[in] nodename   node path holding the x y z structure to be parsed by readXML
	 */
	Coordinate3D(XMLfileUnits& xmlconfig, std::string nodename = std::string("."));

	/** @brief Read in XML configuration for x y z coordinate.
	 *
	 * The following xml object structure is handled by this method:
	 * \code{.xml}
	   <x>DOUBLE</x>
	   <y>DOUBLE</y>
	   <z>DOUBLE</z>
	   \endcode
	 *  @note values are processed with units
	 */
	void readXML(XMLfileUnits& xmlconfig);

	double x() const { return _vec[0]; } //!< Get x component of coordinate
	double y() const { return _vec[1]; } //!< Get y component of coordinate
	double z() const { return _vec[2]; } //!< Get z component of coordinate


	/** @brief obtain coordinate vector values
	 *  @deprecated Use get() returning std::array<double, 3> instead.
	 */
	[[deprecated("Use get() returning std::array<double,3> instead")]]
	void get(double vec[3]);

	/** @brief obtain coordinate vector values
	 *  @return array containing the x, y, z components (in reduced units)
	 */
	std::array<double, 3> get() const {return _vec; }

private:
	std::array<double, 3> _vec; //!< holds the coordinate vector.
};

#endif  // SRC_UTILS_COORDINATE3D_H_
