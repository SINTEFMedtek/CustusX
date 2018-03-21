/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVECTOR3D_H_
#define CXVECTOR3D_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"


#include "cxMathBase.h"
class QString;

namespace cx
{

/**
 * \addtogroup cx_resource_core_math
 * @{
 */


cxResource_EXPORT bool similar(double a, double b, double tol = 1.0E-6); ///< check for equality with a tolerance: |b-a|<tol

/**\brief Vector3D is a representation of a point or vector in 3D.
 *
 * Vector3D is implemented using the Eigen library type Eigen::Vector3d,
 * and extended with convenience methods using the Eigen plugin system.
 *
 * Some of the extension exist for backward compatibility with
 * the old inhouse Vector3D.
 *
 */
typedef Eigen::Vector3d Vector3D;
typedef boost::shared_ptr<Vector3D> Vector3DPtr;

cxResource_EXPORT Vector3D multiply_elems(const Vector3D& a, const Vector3D& b); ///< perform element-wise multiplication of a and b.
cxResource_EXPORT Vector3D divide_elems(const Vector3D& a, const Vector3D& b); ///< perform element-wise division of a and b.
cxResource_EXPORT Vector3D cross(const Vector3D& a, const Vector3D& b); ///< compute cross product of a and b.
cxResource_EXPORT double dot(const Vector3D& a, const Vector3D& b); ///< compute inner product (or dot product) of a and b.
cxResource_EXPORT bool similar(const Vector3D& a, const Vector3D& b, double tol = 1.0E-6); ///< check for equality with a tolerance: |b-a|<tol
cxResource_EXPORT Vector3D unitVector(double thetaXY, double thetaZ = 0.0); ///< compute a unit vector given angles xy in the xy plane and z meaning the elevation from the xy plane.
cxResource_EXPORT double getThetaXY(Vector3D v); ///< get thetaXY, meaning the angle of v projected onto the xy plane
cxResource_EXPORT double getThetaZ(Vector3D v); ///< get thetaZ, z meaning the elevation from the xy plane

cxResource_EXPORT Vector3D round(const Vector3D& a);
cxResource_EXPORT Vector3D ceil(const Vector3D& a);

cxResource_EXPORT bool similar(const Eigen::Array3i& a, const Eigen::Array3i& b); ///< identical to the equality operator, exists for symmetry with Vector3D.

cxResource_EXPORT QString prettyFormat(Vector3D val, int decimals=1, int fieldWidth=3);

cxResource_EXPORT Eigen::Vector2d fromString(const QString& text);

/**
 * @}
 */

} // namespace cx
// --------------------------------------------------------

#endif /*CXVECTOR3D_H_*/
