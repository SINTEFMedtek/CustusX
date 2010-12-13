#ifndef SSCTRANSFORM3D_H_
#define SSCTRANSFORM3D_H_

#include <boost/shared_ptr.hpp>
#include <QString>
#include "vtkForwardDeclarations.h"
#include "sscVector3D.h"

// --------------------------------------------------------
namespace ssc
{
//namespace utils
//{
// --------------------------------------------------------
class DoubleBoundingBox3D;

/**Transform 3D vectors and coordinates.
 * Wraps a vtkMatrix4x4.
 */
class Transform3D
{
private:
	/**Helper for accessing a single value using M[i][j] notation */
	class ElementProxy
	{
	public:
		ElementProxy(vtkMatrix4x4Ptr matrix, unsigned row, unsigned col);
		void operator=(double val);
		operator double() const;
	private:
		unsigned mRow, mCol;
		vtkMatrix4x4Ptr mMatrix;
	};
	/**Helper for accessing a single value using M[i][j] notation */
	class RowProxy
	{
	public:
		RowProxy(vtkMatrix4x4Ptr matrix, unsigned row);
		ElementProxy operator[](unsigned col);
		const ElementProxy operator[](unsigned col) const;
	private:
		unsigned mRow;
		vtkMatrix4x4Ptr mMatrix;
	};

public:
	Transform3D();
	explicit Transform3D(const double* data);      ///< construct a transform matrix from a c-style array of 16 numbers, vtk ordering
  static Transform3D fromString(const QString& text, bool* ok=0); ///< construct a transform matrix from a string containing 16 whitespace-separated numbers, vtk ordering
	explicit Transform3D(vtkMatrix4x4* m);
	Transform3D(const Transform3D& t);
	Transform3D& operator=(const Transform3D& t);
	virtual ~Transform3D();

	Vector3D vector(const Vector3D& v) const;      ///< transform a free vector [x,y,z,0]
	Vector3D unitVector(const Vector3D& v) const;  ///< transform a unit vector [x,y,z,0], force |v|=1 after transform.
	Vector3D coord(const Vector3D& v) const;       ///< transform a coordinate [x,y,z,1].
	Transform3D inv() const;                       ///< return an inverted transform M^-1
	void operator*=(const Transform3D& rhs);       ///< multiply two transforms as if they were matrices
	std::ostream& put(std::ostream& s, int indent=0, char newline='\n') const;
	boost::array<double, 16> flatten() const;      ///< return matrix as a flat array, vtk ordering

	RowProxy operator[](unsigned row);
	const RowProxy operator[](unsigned row) const;

	vtkMatrix4x4Ptr matrix();
	vtkMatrix4x4Ptr matrix() const;

private:
	vtkMatrix4x4Ptr mMatrix;
};

std::ostream& operator<<(std::ostream& s, const Transform3D& t);
bool similar(const Transform3D& a, const Transform3D& b, double tol=1.0E-4);

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
typedef boost::shared_ptr<Transform3D> Transform3DPtr;
//} // namespace utils
//using namespace utils;

} // namespace ssc
// --------------------------------------------------------

#endif /*SSCTRANSFORM3D_H_*/
