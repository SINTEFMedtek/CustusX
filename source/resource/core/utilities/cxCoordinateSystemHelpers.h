#ifndef CXCOORDINATESYSTEMHELPERS_H_
#define CXCOORDINATESYSTEMHELPERS_H_

//#include "cxTransform3D.h"
#include "cxDefinitions.h"
#include "cxForwardDeclarations.h"
#include <vector>
#include <QString>

namespace cx
{

/**\brief Identification of a Coordinate system.
 *
 * \sa CoordinateSystemProvider
 *
 * \ingroup cx_resource_core_utilities
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
#endif /* CXCOORDINATESYSTEMHELPERS_H_ */
