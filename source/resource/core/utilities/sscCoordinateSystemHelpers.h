#ifndef SSCCOORDINATESYSTEMHELPERS_H_
#define SSCCOORDINATESYSTEMHELPERS_H_

#include "sscTransform3D.h"
#include "sscDefinitions.h"
#include "sscForwardDeclarations.h"
#include <vector>

namespace cx
{

/**\brief Identification of a Coordinate system.
 *
 * \sa CoordinateSystemProvider
 *
 * \ingroup sscUtility
 */
struct CoordinateSystem
{
    CoordinateSystem() : mId(csCOUNT) {}
	explicit CoordinateSystem(COORDINATE_SYSTEM id, QString ref="") : mId(id), mRefObject(ref) {}
	COORDINATE_SYSTEM mId; ///< the type of coordinate system
	QString           mRefObject; ///< for tool, sensor and data we need a object uid to define the coordinate system

	static CoordinateSystem reference() { return CoordinateSystem(csREF); }
	static CoordinateSystem patientReference() { return CoordinateSystem(csPATIENTREF); }
	QString toString() const;
	static CoordinateSystem fromString(QString text);

};
bool operator==(const CoordinateSystem& lhs, const CoordinateSystem& rhs);
typedef CoordinateSystem Space;

}//namespace cx
#endif /* SSCCOORDINATESYSTEMHELPERS_H_ */
