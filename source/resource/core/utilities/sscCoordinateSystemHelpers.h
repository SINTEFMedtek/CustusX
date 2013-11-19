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
 * \sa CoordinateSystemHelpers
 *
 * \ingroup sscUtility
 */
struct CoordinateSystem
{
    CoordinateSystem() : mId(csCOUNT) {}
	explicit CoordinateSystem(COORDINATE_SYSTEM id, QString ref="") : mId(id), mRefObject(ref) {}
	COORDINATE_SYSTEM mId; ///< the type of coordinate system
	QString           mRefObject; ///< for tool, sensor and data we need a object uid to define the coordinate system

	QString toString() const;
	static CoordinateSystem fromString(QString text);

};
bool operator==(const CoordinateSystem& lhs, const CoordinateSystem& rhs);
typedef CoordinateSystem Space;

/**\brief Helper class that gets you from any coordinate system to any
 * other coordinate system in CustusX
 *
 * \sa \ref ssc_page_coords
 *
 * \date 3. nov. 2010
 * \author: Janne Beate Bakeng
 *
 * \ingroup sscUtility
 */
class CoordinateSystemHelpers
{
public:
	CoordinateSystemHelpers(){};

	static Vector3D getDominantToolTipPoint(CoordinateSystem to, bool useOffset = false); ///< P_to, dominant tools current point in coord
    static Transform3D getDominantToolTipTransform(CoordinateSystem to, bool useOffset = false);///< Get toMt, where t is dominant tool
	static Transform3D get_toMfrom(CoordinateSystem from, CoordinateSystem to); ///< to_M_from

	static CoordinateSystem getS(ToolPtr tool); ///<tools sensor coordinate system
	static CoordinateSystem getT(ToolPtr tool); ///<tools coordinate system
	static CoordinateSystem getTO(ToolPtr tool); ///<tool offset coordinate system
	static CoordinateSystem getD(DataPtr data);///<datas coordinate system	static CoordinateSystem getPr(); ///<patient references coordinate system
	static CoordinateSystem getPr();
	static CoordinateSystem getR(); ///<data references coordinate system

	static std::vector<CoordinateSystem> getSpacesToPresentInGUI();

private:
	static Transform3D get_rMfrom(CoordinateSystem from); ///< ref_M_from

	static Transform3D get_rMr(); ///< ref_M_ref
	static Transform3D get_rMd(QString uid);
	static Transform3D get_rMdv(QString uid);
	static Transform3D get_rMpr();
	static Transform3D get_rMt(QString uid);
	static Transform3D get_rMto(QString uid);
	static Transform3D get_rMs(QString uid);

	static CoordinateSystem getToolCoordinateSystem(ToolPtr tool);
};
typedef CoordinateSystemHelpers SpaceHelpers;
}//namespace cx
#endif /* SSCCOORDINATESYSTEMHELPERS_H_ */
