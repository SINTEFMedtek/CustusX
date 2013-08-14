#ifndef SSCTRANSFORM3D_H_
#define SSCTRANSFORM3D_H_

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <QString>
#include "sscVector3D.h"

/** implementation functions used the Eigen extensions.
 *
 */
namespace ssc_transform3D_internal
{
boost::array<double, 16> flatten(const Eigen::Affine3d* self);
void fill(Eigen::Affine3d* self, vtkMatrix4x4Ptr m);
void fill(Eigen::Affine3d* self, const double* raw);
vtkMatrix4x4Ptr getVtkMatrix(const Eigen::Affine3d* self);
std::ostream& put(const Eigen::Affine3d* self, std::ostream& s, int indent, char newline);
Eigen::Affine3d fromString(const QString& text, bool* _ok);
}

namespace Eigen
{

template<typename _Scalar, int _Dim, int _Mode, int _Options>
std::ostream& operator<<(std::ostream& s, const Eigen::Transform<_Scalar, _Dim, _Mode, _Options>& t)
{
	s << t.matrix().format(IOFormat()); // hack: force OK output even when the default in sscMathBase.h is Veector3D-centered.
	//  t.put(s);
	return s;
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Vector3d Transform<_Scalar, _Dim, _Mode, _Options>::vector(const Vector3d& v) const ///< transform a free vector [x,y,z,0]
{
	return this->linear() * v;
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Vector3d Transform<_Scalar, _Dim, _Mode, _Options>::unitVector(const Vector3d& v) const
{
	return (this->linear() * v).normal();
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Vector3d Transform<_Scalar, _Dim, _Mode, _Options>::coord(const Vector3d& v) const
{
	return (*this) * v;
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform<_Scalar, _Dim, _Mode, _Options> Transform<_Scalar, _Dim, _Mode, _Options>::inv() const
{
	return this->inverse();
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
boost::array<double, 16> Transform<_Scalar, _Dim, _Mode, _Options>::flatten() const
{
	return ssc_transform3D_internal::flatten(this);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform<_Scalar, _Dim, _Mode, _Options>::Transform(vtkMatrix4x4* m)
{
	ssc_transform3D_internal::fill(this, m);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform<_Scalar, _Dim, _Mode, _Options>::Transform(double* raw)
{
	ssc_transform3D_internal::fill(this, raw);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
vtkMatrix4x4Ptr Transform<_Scalar, _Dim, _Mode, _Options>::getVtkMatrix() const
{
	return ssc_transform3D_internal::getVtkMatrix(this);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
std::ostream& Transform<_Scalar, _Dim, _Mode, _Options>::put(std::ostream& s, int indent, char newline) const
{
	return ssc_transform3D_internal::put(this, s, indent, newline);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform<_Scalar, _Dim, _Mode, _Options> Transform<_Scalar, _Dim, _Mode, _Options>::fromString(const QString& text,
	bool* _ok)
{
	return ssc_transform3D_internal::fromString(text, _ok);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform<_Scalar, _Dim, _Mode, _Options> Transform<_Scalar, _Dim, _Mode, _Options>::fromVtkMatrix(vtkMatrix4x4Ptr m)
{
	Transform<_Scalar, _Dim, _Mode, _Options> retval;
	ssc_transform3D_internal::fill(&retval, m);
	return retval;
}

} // namespace Eigen

// --------------------------------------------------------
namespace ssc
{
class DoubleBoundingBox3D;

/**
 * \addtogroup sscMath
 * @{
 */

/**\brief Transform3D is a representation of an affine 3D transform.
 *
 * Transform3D is implemented using the Eigen library type Eigen::Affine3d,
 * and extended with convenience methods using the Eigen plugin system.
 *
 * Some of the extension exist for backward compatibility with
 * the old inhouse Transform3D.
 *
 */
typedef Eigen::Affine3d Transform3D;

bool similar(const Transform3D& a, const Transform3D& b, double tol = 1.0E-4);

DoubleBoundingBox3D transform(const Transform3D& m, const DoubleBoundingBox3D& bb);

Transform3D createTransformNormalize(const DoubleBoundingBox3D& in, const DoubleBoundingBox3D& out);
Transform3D createTransformScale(const Vector3D& scale);
Transform3D createTransformTranslate(const Vector3D& translation);
Transform3D createTransformRotateX(const double angle);
Transform3D createTransformRotateY(const double angle);
Transform3D createTransformRotateZ(const double angle);
Transform3D createTransformIJC(const Vector3D& ivec, const Vector3D& jvec, const Vector3D& center);

// --------------------------------------------------------
typedef boost::shared_ptr<Transform3D> Transform3DPtr;

/**
 * @}
 */

} // namespace ssc
// --------------------------------------------------------

#endif /*SSCTRANSFORM3D_H_*/
