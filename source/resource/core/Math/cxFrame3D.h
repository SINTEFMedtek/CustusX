/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFRAME3D_H_
#define CXFRAME3D_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxTransform3D.h"

// --------------------------------------------------------
namespace cx
{

/**
 * \addtogroup cx_resource_core_math
 * @{
 */


/**\brief Helper class for visualizing rotational angles to a human user.
 *
 * Upon initialization, the angles X,Y,Z are initialized with the Euler XYZ decomposition.
 * Further calls to rotate() will be treated as rotation of that particular unit axis in the
 * target space. Because these rotations are non-commuting, it is impossible to retrieve the
 * angles from the resulting matrix. Instead, they are stored internally.
 * An external change of the matrix (by using reset()) will cause the class to be reset with
 * the Euler angles.
 *
 */
class cxResource_EXPORT DecomposedTransform3D
{
public:
	DecomposedTransform3D();
	DecomposedTransform3D(Transform3D m);
	void reset(Transform3D m); ///< reinitialize with a fresh matrix.
	void setAngles(Vector3D xyz);
	void setPosition(Vector3D pos);
	Vector3D getAngles() const;
	Vector3D getPosition() const;
	Transform3D getMatrix() const;
private:
	Vector3D mAngle; ///< x,y,z rotation angles
	Vector3D mPos; ///< x,y,z position
	Transform3D m_R; ///< 3x3 rotational matrix
};

/**\brief Defines an axis-angle representation of a position+orientation in 3D space.
 *
 * - ThetaXY+ThetaZ is the axis of rotation
 * - Phi is the amount of rotation
 * - P is the translation
 * 
 * The frame describes a coordinate space B related to A.
 * The transform for the frame F_B, T_B, can be used to convert points
 * in B to points in A:
 *    p_B = T_B * p_B
 * 
 */
class cxResource_EXPORT Frame3D
{
public:
	static Frame3D create(const Transform3D& transform);
	Transform3D transform() const;
	Vector3D rotationAxis() const;
	void setRotationAxis(const Vector3D& k);
	void put(std::ostream& s) const;
	void test();

	boost::array<double, 6> getCompactAxisAngleRep() const;
	static Frame3D fromCompactAxisAngleRep(const boost::array<double, 6>& rep);

	Vector3D getEulerXYZ() const;
	void setEulerXYZ(const Vector3D& xyz);

	Frame3D();
	virtual ~Frame3D();

	Eigen::AngleAxisd mAngleAxis; ///< angle-axis representation of rotation
	Vector3D mPos; ///< position

private:
};

cxResource_EXPORT std::ostream& operator<<(std::ostream& s, const Frame3D& t);

/**
 * @}
 */

} // namespace cx
// --------------------------------------------------------

#endif /*CXFRAME3D_H_*/
