#ifndef SSCTRANSFORM3D_H_
#define SSCTRANSFORM3D_H_

#include <boost/shared_ptr.hpp>
#include <vtkSmartPointer.h>
#include "sscVector3D.h"
#include "sscBoundingBox3D.h"

typedef vtkSmartPointer<class vtkMatrix4x4> vtkMatrix4x4Ptr;

// --------------------------------------------------------
namespace ssc
{

namespace utils
{
// --------------------------------------------------------

/**Transform 3D vectors and coordinates.
 * Wraps a vtkMatrix4x4.
 */
class Transform3D
{
public:
	Transform3D();
	explicit Transform3D(const double* data);
	explicit Transform3D(vtkMatrix4x4* m);
	Transform3D(const Transform3D& t);
	Transform3D& operator=(const Transform3D& t);
	virtual ~Transform3D();

	Vector3D vector(const Vector3D& v) const; ///< transform a free vector [x,y,z,0]
	Vector3D unitVector(const Vector3D& v) const; ///< transform a unit vector [x,y,z,0], force |v|=1 after transform.
	Vector3D coord(const Vector3D& v) const; ///< transform a coordinate [x,y,z,1].
	Transform3D inv() const; ///< return an inverted transform M^-1
	void operator*=(const Transform3D& rhs); ///< multiply two transforms as if they were matrices
	std::ostream& put(std::ostream& s, int indent=0, char newline='\n') const;
	boost::array<double, 16> flatten() const;

//	Vector3D getIVector() const; ///< get the first column vector
//	Vector3D getJVector() const; ///< get the second column vector
//	Vector3D getKVector() const; ///< get the third column vector
//	Vector3D getOrigin() const; ///< get the third column vector, center for a orhonormal affine transform
//	Vector3D getColumnVector(unsigned col) const;

	vtkMatrix4x4Ptr matrix();
	vtkMatrix4x4Ptr matrix() const;

private:
	vtkMatrix4x4Ptr mMatrix;
};

std::ostream& operator<<(std::ostream& s, const Transform3D& t);
bool similar(const Transform3D& a, const Transform3D& b, double tol=1.0E-6);

DoubleBoundingBox3D transform(const Transform3D& m, const DoubleBoundingBox3D& bb);

Transform3D operator*(const Transform3D& lhs, const Transform3D& rhs); ///< multiply two transforms as if they were matrices

Transform3D createTransformNormalize(const DoubleBoundingBox3D& in, const DoubleBoundingBox3D& out);
Transform3D createTransformScale(const Vector3D& scale);
Transform3D createTransformTranslate(const Vector3D& translation);
Transform3D createTransformRotateX(const double angle);
Transform3D createTransformRotateY(const double angle);
Transform3D createTransformRotateZ(const double angle);
Transform3D createTransformIJC(const Vector3D& ivec, const Vector3D& jvec, const Vector3D& center);


// --------------------------------------------------------
} // namespace utils
typedef boost::shared_ptr<Transform3D> Transform3DPtr;
} // namespace ssc
// --------------------------------------------------------

#endif /*SSCTRANSFORM3D_H_*/
