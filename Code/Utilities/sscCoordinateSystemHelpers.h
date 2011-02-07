#ifndef SSCCOORDINATESYSTEMHELPERS_H_
#define SSCCOORDINATESYSTEMHELPERS_H_

#include "sscTransform3D.h"
#include "sscDefinitions.h"
#include "sscForwardDeclarations.h"

namespace ssc
{
struct CoordinateSystem
{
  COORDINATE_SYSTEM mId; ///< the type of coordinate system
  QString           mRefObject; ///< for tool, sensor and data we need a object uid to define the coordinate system
};
typedef CoordinateSystem Space;

/**
 * \class CoordinateSystemHelpers
 *
 * \brief Helper class that gets you from any coordinate system to any
 * other coordinate system in CustusX
 *
 * \date 3. nov. 2010
 * \author: Janne Beate Bakeng
 */
class CoordinateSystemHelpers
{
public:
  CoordinateSystemHelpers(){};

  static Vector3D getDominantToolTipPoint(CoordinateSystem to, bool useOffset = false); ///< P_to, dominant tools current point in coord
  static Transform3D get_toMfrom(CoordinateSystem from, CoordinateSystem to); ///< to_M_from

  static ssc::CoordinateSystem getS(ssc::ToolPtr tool); ///<tools sensor coordinate system
  static ssc::CoordinateSystem getT(ssc::ToolPtr tool); ///<tools coordinate system
  static ssc::CoordinateSystem getD(ssc::DataPtr data);///<datas coordinate system
  static ssc::CoordinateSystem getPr(); ///<patient references coordinate system
  static ssc::CoordinateSystem getR(); ///<data references coordinate system

private:
  Transform3D get_rMfrom(CoordinateSystem from) const; ///< ref_M_from

  Transform3D get_rMr() const; ///< ref_M_ref
  Transform3D get_rMd(QString uid) const; ///< ref_M_d
  Transform3D get_rMpr() const; ///< ref_M_pr
  Transform3D get_rMt(QString uid) const; ///< ref_M_t
  Transform3D get_rMs(QString uid) const; ///< ref_M_s
};
typedef CoordinateSystemHelpers SpaceHelpers;
}//namespace ssc
#endif /* SSCCOORDINATESYSTEMHELPERS_H_ */
