#include "sscTransform3D.h"

#include <sstream>
#include <vector>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
typedef vtkSmartPointer<class vtkTransform> vtkTransformPtr;
#include "sscTypeConversions.h"

#include "sscBoundingBox3D.h"

// --------------------------------------------------------
namespace ssc
{
namespace utils
{
// --------------------------------------------------------

/**creates a  transform from double[16].
 */
Transform3D::Transform3D(const double* data)
{
	mMatrix = vtkMatrix4x4Ptr::New();
	mMatrix->DeepCopy(data);
}

Transform3D::Transform3D(vtkMatrix4x4* m)
{
	mMatrix = vtkMatrix4x4Ptr(m);
}

Transform3D Transform3D::fromString(const QString& text, bool* ok)
{
  std::vector<double> raw = convertQString2DoubleVector(text, ok);
  if (raw.size()!=16)
    *ok = false;
  if (!ok)
    return Transform3D();
  return Transform3D((double*)&(*raw.begin()));
}

Transform3D::Transform3D()
{
	mMatrix = vtkMatrix4x4Ptr::New();
	mMatrix->Identity();
}
Transform3D::Transform3D(const Transform3D& t)
{
	mMatrix = vtkMatrix4x4Ptr::New();
	mMatrix->DeepCopy(t.mMatrix.GetPointer());	
}
Transform3D& Transform3D::operator=(const Transform3D& t)
{
	if (&t!=this)
	{
		mMatrix = vtkMatrix4x4Ptr::New();
		mMatrix->DeepCopy(t.mMatrix.GetPointer());	
	}
	return *this;
}
Transform3D::~Transform3D()
{
}

//Vector3D Transform3D::getColumnVector(unsigned col) const
//{
//	return Vector3D(mMatrix->GetElement(0,col),
//	                mMatrix->GetElement(1,col),
//	                mMatrix->GetElement(2,col));
//}
//
//Vector3D Transform3D::getIVector() const
//{
//	return getColumnVector(0);
//}
//Vector3D Transform3D::getJVector() const
//{
//	return getColumnVector(1);
//}
//Vector3D Transform3D::getKVector() const
//{
//	return getColumnVector(2);
//}
//Vector3D Transform3D::getOrigin() const
//{
//	return getColumnVector(3);
//}

/**hack. fix soooon.
 */
vtkMatrix4x4Ptr Transform3D::matrix() const
{
	return mMatrix;
}

vtkMatrix4x4Ptr Transform3D::matrix()
{
	return mMatrix;
}


std::ostream& Transform3D::put(std::ostream& s, int indent, char newline) const
{
	std::string ind(indent, ' ');

	std::ostringstream ss; // avoid changing state of input stream
	ss << setprecision(3) << std::fixed;
	
	for (unsigned i=0; i<4; ++i)
	{
		ss << ind;
		for (unsigned j=0; j<4; ++j)
		{
			ss << setw(10) << mMatrix->GetElement(i,j) << " ";
		}
		if (i!=3)
		{
			ss << newline;
		}
	}
	
	s << ss.str();
	
	return s;
}

/**Return the matrix a a flat array with 16 elements.
 * 
 */
boost::array<double, 16> Transform3D::flatten() const
{
	boost::array<double, 16> retval;
	for (unsigned i=0; i<4; ++i)
	{
		for (unsigned j=0; j<4; ++j)
		{
			retval[i*4+j] = mMatrix->GetElement(i,j);
		}
	}
	return retval;
}


std::ostream& operator<<(std::ostream& s, const Transform3D& t)
{
	return t.put(s, 0, '\n');
}

Vector3D Transform3D::vector(const Vector3D& v) const
{
	double pt[4] = {v[0],v[1],v[2],0};
	mMatrix->MultiplyPoint(pt,pt);
	return Vector3D(pt);	
}

Vector3D Transform3D::unitVector(const Vector3D& v) const
{
	double pt[4] = {v[0],v[1],v[2],0};
	mMatrix->MultiplyPoint(pt,pt);
	return Vector3D(pt);		
}

Vector3D Transform3D::coord(const Vector3D& v) const
{
	double pt[4] = {v[0],v[1],v[2],1};
	mMatrix->MultiplyPoint(pt,pt);
	return Vector3D(pt);		
}

Transform3D Transform3D::inv() const
{
	Transform3D retval;
	vtkMatrix4x4::Invert(mMatrix, retval.mMatrix);
	return retval;
}

void Transform3D::operator*=(const Transform3D& rhs)
{
	vtkMatrix4x4::Multiply4x4(mMatrix, rhs.mMatrix, mMatrix);
}

Transform3D operator*(const Transform3D& lhs, const Transform3D& rhs)
{
	Transform3D tmp(lhs);
	tmp *= rhs;
	return tmp;
}

bool similar(const Transform3D& a, const Transform3D& b, double tol)
{
	vtkMatrix4x4Ptr m = a.matrix();
	vtkMatrix4x4Ptr n = b.matrix();
	for (int i=0; i<4; ++i)
		for (int j=0; j<4; ++j)
			if (!similar(m->GetElement(i,j), n->GetElement(i,j), tol))
			{
				return false;
			}
	return true;
}
// --------------------------------------------------------

/**Transform bb using the transform m.
 * Only the two defining corners are actually transformed.
 */
DoubleBoundingBox3D transform(const Transform3D& m, const DoubleBoundingBox3D& bb)
{
	Vector3D a = m.coord(bb.bottomLeft());
	Vector3D b = m.coord(bb.topRight());
	return DoubleBoundingBox3D(a,b);
}

/**Create a transform representing a scale in x,y,z
 */
Transform3D createTransformScale(const Vector3D& scale)
{
	vtkTransformPtr transform = vtkTransformPtr::New();
	transform->Identity();
	transform->Scale(scale.begin());
	return Transform3D(transform->GetMatrix());
}

/**Create a transform representing a translation
 */
Transform3D createTransformTranslate(const Vector3D& translation)
{
	vtkTransformPtr transform = vtkTransformPtr::New();
	transform->Identity();
	transform->Translate(translation.begin());
	return Transform3D(transform->GetMatrix());	
}

/**Create a transform representing a rotation about the X-axis with an input angle.
 */
Transform3D createTransformRotateX(const double angle)
{
	double angRad = angle/M_PI*180.0;
	vtkTransformPtr transform = vtkTransformPtr::New();
	transform->Identity();
	transform->RotateX(angRad);
	return Transform3D(transform->GetMatrix());		
}

/**Create a transform representing a rotation about the Y-axis with an input angle.
 */
Transform3D createTransformRotateY(const double angle)
{
	double angRad = angle/M_PI*180.0;
	vtkTransformPtr transform = vtkTransformPtr::New();
	transform->Identity();
	transform->RotateY(angRad);
	return Transform3D(transform->GetMatrix());		
}

/**Create a transform representing a rotation about the Z-axis with an input angle.
 */
Transform3D createTransformRotateZ(const double angle)
{
	double angRad = angle/M_PI*180.0;
	vtkTransformPtr transform = vtkTransformPtr::New();
	transform->Identity();
	transform->RotateZ(angRad);
	return Transform3D(transform->GetMatrix());		
}


/**Normalize volume defined by in to volume defined by out.
 *
 * This is intended for creating transforms from one viewport to another, i.e.
 * the two boxes should be aligned and differ only in translation + scaling.
 */
Transform3D createTransformNormalize(const DoubleBoundingBox3D& in, const DoubleBoundingBox3D& out)
{
	//std::ostringstream stream;
	// translate input bottomleft to origin, scale, translate back to output bottomleft.
	Transform3D T0 = createTransformTranslate(-in.bottomLeft());
	Vector3D inrange = in.range();
	Vector3D outrange = out.range();
	Vector3D scale;
	// check for zero input dimensions
	for (unsigned i=0; i<scale.size(); ++i)
	{
		if (fabs(inrange[i])<1.0E-5)
			scale[i] = 0;
		else
			scale[i] = outrange[i] / inrange[i];
	}
	Transform3D S = createTransformScale(scale);
	Transform3D T1 = createTransformTranslate(out.bottomLeft());
	Transform3D M = T1*S*T0;
	return M;
}

/**Create a transform to a space defined by an origin and two perpendicular unit vectors that
 * for the x-y plane.
 * The original space is A and the space defined by ijc are B
 * The returned transform M_AB converts a point in B to A:
 * 		p_A = M_AB * p_B  
 */
Transform3D createTransformIJC(const Vector3D& ivec, const Vector3D& jvec, const Vector3D& center)
{
	Vector3D kvec = cross(ivec,jvec);
	// set all column vectors
	vtkMatrix4x4Ptr matrix = vtkMatrix4x4Ptr::New();
	matrix->Identity();
	for (unsigned i=0; i<3; ++i)
	{
		matrix->SetElement(i, 0, ivec[i]);
		matrix->SetElement(i, 1, jvec[i]);
		matrix->SetElement(i, 2, kvec[i]);
		matrix->SetElement(i, 3, center[i]);
	}
	return Transform3D(matrix);
}



Transform3D::ElementProxy::ElementProxy(vtkMatrix4x4Ptr matrix, unsigned row, unsigned col)
{
	mMatrix = matrix;
	mRow = row;
	mCol = col;
}

void Transform3D::ElementProxy::operator=(double val)
{
	mMatrix->SetElement(mRow, mCol, val);
}

Transform3D::ElementProxy::operator double() const
{
	return mMatrix->GetElement(mRow, mCol);
}

Transform3D::RowProxy::RowProxy(vtkMatrix4x4Ptr matrix, unsigned row)
{
	mMatrix = matrix;
	mRow = row;
}

Transform3D::ElementProxy Transform3D::RowProxy::operator[](unsigned col)
{
	return ElementProxy(mMatrix, mRow, col);
}
const Transform3D::ElementProxy Transform3D::RowProxy::operator[](unsigned col) const
{
	return ElementProxy(mMatrix, mRow, col);
}

Transform3D::RowProxy Transform3D::operator[](unsigned row) 
{
	return RowProxy(mMatrix, row);
}
const Transform3D::RowProxy Transform3D::operator[](unsigned row) const
{
	return RowProxy(mMatrix, row);
}


// --------------------------------------------------------
} // namespace utils
} // namespace ssc
// --------------------------------------------------------
