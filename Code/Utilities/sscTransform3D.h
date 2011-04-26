#ifndef SSCTRANSFORM3D_H_
#define SSCTRANSFORM3D_H_

#include "boost/array.hpp"
#include <boost/shared_ptr.hpp>
#include <QString>
#include "vtkForwardDeclarations.h"
#include "sscVector3D.h"

namespace Eigen
{
  std::ostream& operator<<(std::ostream& s, const Affine3d& t);
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
  boost::array<double, 16> retval;
  std::copy(this->data(), this->data()+16, retval.begin());
  return retval;
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform< _Scalar, _Dim, _Mode, _Options >::Transform(vtkMatrix4x4* m)
{
  for (int r=0; r<4; ++r)
    for (int c=0; c<4; ++c)
      (*this)(r,c) = m->GetElement(r,c);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
vtkMatrix4x4Ptr Transform< _Scalar, _Dim, _Mode, _Options >::getVtkMatrix() const
{
  vtkMatrix4x4Ptr m = vtkMatrix4x4Ptr::New();

  for (int r=0; r<4; ++r)
    for (int c=0; c<4; ++c)
      m->SetElement(r,c, (*this)(r,c));;

  return m;
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform< _Scalar, _Dim, _Mode, _Options > Transform< _Scalar, _Dim, _Mode, _Options >::fromString(const QString& text, bool* _ok)
{
  bool okval = false; // if input _ok is null, we still need a flag
  bool* ok = &okval;
  if (_ok)
    ok = _ok;

  std::vector<double> raw = convertQString2DoubleVector(text, ok);
  if (raw.size()!=16)
    *ok = false;
  if (!ok)
    return Transform3D();
  return Transform3D((double*)&(*raw.begin()));
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
