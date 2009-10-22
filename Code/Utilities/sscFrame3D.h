#ifndef SSCFRAME3D_H_
#define SSCFRAME3D_H_

#include "sscTransform3D.h"

// --------------------------------------------------------
namespace ssc
{

namespace utils
{
// --------------------------------------------------------

/**Defines an axis-angle representation of a position+orientation in 3D space.
 * - ThetaXY+ThetaZ is the axis of rotation
 * - Phi is the amount of rotation
 * - P is the position of the origin
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
	
	Frame3D();
	virtual ~Frame3D();
	
private:
	Transform3D generateRotationMatrix() const;
	
    double mThetaXY; // angle of axis from x in xy plane
    double mThetaZ;  // angle of axis from xy-plane toward z-axis. (thetaZ=0 -> we are in xy-plane)
    double mPhi;     // rotation around axis.
    Vector3D mPos;         // W==1

};

std::ostream& operator<<(std::ostream& s, const Frame3D& t);


// --------------------------------------------------------
} // namespace utils
} // namespace ssc
// --------------------------------------------------------

#endif /*SSCFRAME3D_H_*/
