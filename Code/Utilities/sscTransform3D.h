#ifndef SSCTRANSFORM3D_H_
#define SSCTRANSFORM3D_H_

#include "boost/array.hpp"
#include <boost/shared_ptr.hpp>
#include <QString>
#include "vtkForwardDeclarations.h"
#include "sscVector3D.h"
#include "vtkMatrix4x4.h"
#include <vector>
#include "sscTypeConversions.h"

//namespace Eigen
//{
//  std::ostream& operator<<(std::ostream& s, const Affine3d& t);
//}

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
std::ostream& operator<<(std::ostream& s, const Eigen::Transform< _Scalar, _Dim, _Mode, _Options >& t)
{
  t.put(s);
  return s;
}

//std::ostream& operator<<(std::ostream& s, const Transform< _Scalar, _Dim, _Mode, _Options >& t);

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Vector3d Transform< _Scalar, _Dim, _Mode, _Options >::vector(const Vector3d& v) const      ///< transform a free vector [x,y,z,0]
{
  return this->linear() * v;
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Vector3d Transform< _Scalar, _Dim, _Mode, _Options >::unitVector(const Vector3d& v) const
{
  return (this->linear() * v).normal();
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Vector3d Transform< _Scalar, _Dim, _Mode, _Options >::coord(const Vector3d& v) const
{
  return (*this) * v;
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform< _Scalar, _Dim, _Mode, _Options > Transform< _Scalar, _Dim, _Mode, _Options >::inv() const
{
  return this->inverse();
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
boost::array<double, 16> Transform< _Scalar, _Dim, _Mode, _Options >::flatten() const
{
  return ssc_transform3D_internal::flatten(this);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform< _Scalar, _Dim, _Mode, _Options >::Transform(vtkMatrix4x4* m)
{
  ssc_transform3D_internal::fill(this, m);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform< _Scalar, _Dim, _Mode, _Options >::Transform(double* raw)
{
  ssc_transform3D_internal::fill(this, raw);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
vtkMatrix4x4Ptr Transform< _Scalar, _Dim, _Mode, _Options >::getVtkMatrix() const
{
  return ssc_transform3D_internal::getVtkMatrix(this);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
std::ostream& Transform< _Scalar, _Dim, _Mode, _Options >::put(std::ostream& s, int indent, char newline) const
{
  return ssc_transform3D_internal::put(this, s, indent, newline);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform< _Scalar, _Dim, _Mode, _Options > Transform< _Scalar, _Dim, _Mode, _Options >::fromString(const QString& text, bool* _ok)
{
  return ssc_transform3D_internal::fromString(text, _ok);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform< _Scalar, _Dim, _Mode, _Options > Transform< _Scalar, _Dim, _Mode, _Options >::fromVtkMatrix(vtkMatrix4x4Ptr m)
{
  Transform< _Scalar, _Dim, _Mode, _Options > retval;
  ssc_transform3D_internal::fill(&retval, m);
  return retval;
}


//std::ostream& put(std::ostream& s, int indent=0, char newline='\n') const;
//static Transform fromString(const QString& text, bool* ok=0); ///< construct a transform matrix from a string containing 16 whitespace-separated numbers, vtk ordering


}

// define to remove methods not compatible with the eigen library
#define PREPARE_EIGEN_SUPPORT

// --------------------------------------------------------
namespace ssc
{
//namespace utils
//{
// --------------------------------------------------------
class DoubleBoundingBox3D;

/**Use math library Eigen for the the ssc type Transform3D,
 * extended using the Eigen plugin system.
 *
 */
typedef Eigen::Affine3d Transform3D;

///**Transform 3D vectors and coordinates.
// * Wraps a vtkMatrix4x4.
// */
//class Transform3D
//{
//private:
//	/**Helper for accessing a single value using M[i][j] notation */
//	class ElementProxy
//	{
//	public:
//		ElementProxy(vtkMatrix4x4Ptr matrix, unsigned row, unsigned col);
//		void operator=(double val);
//		operator double() const;
//	private:
//		unsigned mRow, mCol;
//		vtkMatrix4x4Ptr mMatrix;
//	};
//	/**Helper for accessing a single value using M[i][j] notation */
//	class RowProxy
//	{
//	public:
//		RowProxy(vtkMatrix4x4Ptr matrix, unsigned row);
//		ElementProxy operator[](unsigned col);
//		const ElementProxy operator[](unsigned col) const;
//	private:
//		unsigned mRow;
//		vtkMatrix4x4Ptr mMatrix;
//	};
//
//public:
//	Transform3D();
//	explicit Transform3D(const double* data);      ///< construct a transform matrix from a c-style array of 16 numbers, vtk ordering
////	QString toString();			///< REMOVED: not symmetric with fromString(). Use operator<< instead. ///< Construct a single-line string representation of the matrix
//	static Transform3D fromString(const QString& text, bool* ok=0); ///< construct a transform matrix from a string containing 16 whitespace-separated numbers, vtk ordering
//	explicit Transform3D(vtkMatrix4x4* m);
//	static Transform3D fromVtkMatrix(vtkMatrix4x4Ptr m);
//	Transform3D(const Transform3D& t);
//	Transform3D& operator=(const Transform3D& t);
//	virtual ~Transform3D();
//
//	Vector3D vector(const Vector3D& v) const;      ///< transform a free vector [x,y,z,0]
//	Vector3D unitVector(const Vector3D& v) const;  ///< transform a unit vector [x,y,z,0], force |v|=1 after transform.
//	Vector3D coord(const Vector3D& v) const;       ///< transform a coordinate [x,y,z,1].
//	Transform3D inv() const;                       ///< return an inverted transform M^-1
//	void operator*=(const Transform3D& rhs);       ///< multiply two transforms as if they were matrices
//	std::ostream& put(std::ostream& s, int indent=0, char newline='\n') const;
//	boost::array<double, 16> flatten() const;      ///< return matrix as a flat array, vtk ordering
//
//#ifndef PREPARE_EIGEN_SUPPORT
//	RowProxy operator[](unsigned row);
//	const RowProxy operator[](unsigned row) const;
//#endif
//
//	ElementProxy operator()(unsigned row, unsigned col);
//  const ElementProxy operator()(unsigned row, unsigned col) const;
//
//  vtkMatrix4x4Ptr getVtkMatrix() const;
//#ifndef PREPARE_EIGEN_SUPPORT
//	vtkMatrix4x4Ptr matrix();
//	vtkMatrix4x4Ptr matrix() const;
//#endif
//
//private:
//	vtkMatrix4x4Ptr mMatrix;
//};

bool similar(const Transform3D& a, const Transform3D& b, double tol=1.0E-4);

DoubleBoundingBox3D transform(const Transform3D& m, const DoubleBoundingBox3D& bb);

//Transform3D operator*(const Transform3D& lhs, const Transform3D& rhs); ///< multiply two transforms as if they were matrices

Transform3D createTransformNormalize(const DoubleBoundingBox3D& in, const DoubleBoundingBox3D& out);
Transform3D createTransformScale(const Vector3D& scale);
Transform3D createTransformTranslate(const Vector3D& translation);
Transform3D createTransformRotateX(const double angle);
Transform3D createTransformRotateY(const double angle);
Transform3D createTransformRotateZ(const double angle);
Transform3D createTransformIJC(const Vector3D& ivec, const Vector3D& jvec, const Vector3D& center);

//std::ostream& operator<<(std::ostream& s, const Transform3D& t);
//std::ostream& operator<<(std::ostream& s, const Eigen::Affine3f& t);
// --------------------------------------------------------
typedef boost::shared_ptr<Transform3D> Transform3DPtr;
//} // namespace utils
//using namespace utils;

} // namespace ssc
// --------------------------------------------------------

//template<typename _Scalar, int _Dim, int _Mode, int _Options>
//std::ostream& operator<<(std::ostream& s, const Eigen::Transform< _Scalar, _Dim, _Mode, _Options >& t);
//
//std::ostream& operator<<(std::ostream& s, const Eigen::Affine3f& t);

#endif /*SSCTRANSFORM3D_H_*/
