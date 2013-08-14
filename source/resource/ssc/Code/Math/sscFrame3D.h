#ifndef SSCFRAME3D_H_
#define SSCFRAME3D_H_

#include "sscTransform3D.h"

// --------------------------------------------------------
namespace ssc
{

/**
 * \addtogroup sscMath
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
class DecomposedTransform3D
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
class Frame3D
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

	ssc::Vector3D getEulerXYZ() const;
	void setEulerXYZ(const ssc::Vector3D& xyz);

	Frame3D();
	virtual ~Frame3D();

	Eigen::AngleAxisd mAngleAxis; ///< angle-axis representation of rotation
	Vector3D mPos; ///< position

private:
};

std::ostream& operator<<(std::ostream& s, const Frame3D& t);

/**
 * @}
 */

} // namespace ssc
// --------------------------------------------------------

#endif /*SSCFRAME3D_H_*/
