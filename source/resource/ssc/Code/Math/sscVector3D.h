#ifndef SSCVECTOR3D_H_
#define SSCVECTOR3D_H_


#include "sscMathBase.h"

namespace ssc
{

/**
 * \addtogroup sscMath
 * @{
 */


bool similar(double a, double b, double tol = 1.0E-6); ///< check for equality with a tolerance: |b-a|<tol

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

Vector3D multiply_elems(const Vector3D& a, const Vector3D& b); ///< perform element-wise multiplication of a and b.
Vector3D divide_elems(const Vector3D& a, const Vector3D& b); ///< perform element-wise division of a and b.
Vector3D cross(const Vector3D& a, const Vector3D& b); ///< compute cross product of a and b.
double dot(const Vector3D& a, const Vector3D& b); ///< compute inner product (or dot product) of a and b.
bool similar(const Vector3D& a, const Vector3D& b, double tol = 1.0E-6); ///< check for equality with a tolerance: |b-a|<tol
Vector3D unitVector(double thetaXY, double thetaZ = 0.0); ///< compute a unit vector given angles xy in the xy plane and z meaning the elevation from the xy plane.
double getThetaXY(Vector3D v); ///< get thetaXY, meaning the angle of v projected onto the xy plane
double getThetaZ(Vector3D v); ///< get thetaZ, z meaning the elevation from the xy plane

Vector3D round(const Vector3D& a);
Vector3D ceil(const Vector3D& a);

bool similar(const Eigen::Array3i& a, const Eigen::Array3i& b); ///< identical to the equality operator, exists for symmetry with Vector3D.

/**
 * @}
 */

} // namespace ssc
// --------------------------------------------------------

#endif /*SSCVECTOR3D_H_*/
