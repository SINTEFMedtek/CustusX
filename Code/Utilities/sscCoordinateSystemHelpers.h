#ifndef SSCCOORDINATESYSTEMHELPERS_H_
#define SSCCOORDINATESYSTEMHELPERS_H_

#include "sscTransform3D.h"
#include "sscDefinitions.h"
#include "sscForwardDeclarations.h"
#include <vector>

namespace ssc
{

/**\brief Identification of a Coordinate system.
 *
 * \sa CoordinateSystemHelpers
 *
 * \ingroup sscUtility
 */
struct CoordinateSystem
{
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
	static Transform3D get_toMfrom(CoordinateSystem from, CoordinateSystem to); ///< to_M_from

	static ssc::CoordinateSystem getS(ssc::ToolPtr tool); ///<tools sensor coordinate system
	static ssc::CoordinateSystem getT(ssc::ToolPtr tool); ///<tools coordinate system
	static ssc::CoordinateSystem getTO(ssc::ToolPtr tool); ///<tool offset coordinate system
	static ssc::CoordinateSystem getD(ssc::DataPtr data);///<datas coordinate system	static ssc::CoordinateSystem getPr(); ///<patient references coordinate system
	static ssc::CoordinateSystem getPr();
	static ssc::CoordinateSystem getR(); ///<data references coordinate system

	static std::vector<CoordinateSystem> getAvailableSpaces();

private:
	static Transform3D get_rMfrom(CoordinateSystem from); ///< ref_M_from

	static Transform3D get_rMr(); ///< ref_M_ref
	static Transform3D get_rMd(QString uid); ///< ref_M_d
	static Transform3D get_rMpr(); ///< ref_M_pr
	static Transform3D get_rMt(QString uid); ///< ref_M_t
	static Transform3D get_rMto(QString uid); ///< t_M_to
	static Transform3D get_rMs(QString uid); ///< ref_M_s
};
typedef CoordinateSystemHelpers SpaceHelpers;
}//namespace ssc
#endif /* SSCCOORDINATESYSTEMHELPERS_H_ */
