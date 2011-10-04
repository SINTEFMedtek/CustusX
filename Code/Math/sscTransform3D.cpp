#include "sscTransform3D.h"

#include <sstream>
#include <vector>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include "sscTypeConversions.h"
#include "sscBoundingBox3D.h"
#include "vtkForwardDeclarations.h"

// --------------------------------------------------------
namespace ssc_transform3D_internal
{

/**provide an array of the transform indices, vtk / row-major ordering
 *
 */
boost::array<double, 16> flatten(const Eigen::Affine3d* self)
{
	boost::array<double, 16> retval;
	boost::array<double, 16>::iterator raw = retval.begin();

	for (int r = 0; r < 4; ++r)
		for (int c = 0; c < 4; ++c)
			*raw++ = (*self)(r, c);

	return retval;
}

void fill(Eigen::Affine3d* self, vtkMatrix4x4Ptr m)
{
	if (!m)
		return;
	for (int r = 0; r < 4; ++r)
		for (int c = 0; c < 4; ++c)
			(*self)(r, c) = m->GetElement(r, c);
}

/**fill the transform with raw data in vtk / row-major ordering form.
 *
 */
void fill(Eigen::Affine3d* self, const double* raw)
{
	for (int r = 0; r < 4; ++r)
		for (int c = 0; c < 4; ++c)
			(*self)(r, c) = *raw++;
}

vtkMatrix4x4Ptr getVtkMatrix(const Eigen::Affine3d* self)
{
	vtkMatrix4x4Ptr m = vtkMatrix4x4Ptr::New();

	for (int r = 0; r < 4; ++r)
		for (int c = 0; c < 4; ++c)
			m->SetElement(r, c, (*self)(r, c));
	;

	return m;
}

std::ostream& put(const Eigen::Affine3d* self, std::ostream& s, int indent, char newline)
{
	QString ind(indent, ' ');

	std::ostringstream ss; // avoid changing state of input stream
	ss << setprecision(3) << std::fixed;

	for (unsigned i = 0; i < 4; ++i)
	{
		ss << ind;
		for (unsigned j = 0; j < 4; ++j)
		{
			ss << setw(10) << (*self)(i, j) << " ";
		}
		if (i != 3)
		{
			ss << newline;
		}
	}

	s << ss.str();

	return s;
}

Eigen::Affine3d fromString(const QString& text, bool* _ok)
{
	bool okval = false; // if input _ok is null, we still need a flag
	bool* ok = &okval;
	if (_ok)
		ok = _ok;

	std::vector<double> raw = convertQString2DoubleVector(text, ok);
	if (raw.size() != 16)
		*ok = false;
	if (!*ok)
		return Eigen::Affine3d();

	Eigen::Affine3d retval;
	fill(&retval, &*raw.begin());
	return retval;
}

} // namespace ssc_transform3D_internal

namespace ssc
{

bool similar(const Transform3D& a, const Transform3D& b, double tol)
{
	boost::array<double, 16> m = a.flatten();
	boost::array<double, 16> n = b.flatten();
	for (int j = 0; j < 16; ++j)
	{
		if (!similar(n[j], m[j], tol))
		{
			return false;
		}
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
	return DoubleBoundingBox3D(a, b);
}

/**Create a transform representing a scale in x,y,z
 */
Transform3D createTransformScale(const Vector3D& scale_)
{
	Transform3D retval = Transform3D::Identity();
	retval.scale(scale_);
	return retval;
}

/**Create a transform representing a translation
 */
Transform3D createTransformTranslate(const Vector3D& translation)
{
	Transform3D retval = Transform3D::Identity();
	retval.translate(translation);
	return retval;
}

/**Create a transform representing a rotation about the X-axis with an input angle.
 */
Transform3D createTransformRotateX(const double angle)
{
	Transform3D retval = Transform3D::Identity();
	retval.rotate(Eigen::AngleAxisd(angle, Vector3D::UnitX()));
	return retval;
}

/**Create a transform representing a rotation about the Y-axis with an input angle.
 */
Transform3D createTransformRotateY(const double angle)
{
	Transform3D retval = Transform3D::Identity();
	retval.rotate(Eigen::AngleAxisd(angle, Vector3D::UnitY()));
	return retval;
}

/**Create a transform representing a rotation about the Z-axis with an input angle.
 */
Transform3D createTransformRotateZ(const double angle)
{
	Transform3D retval = Transform3D::Identity();
	retval.rotate(Eigen::AngleAxisd(angle, Vector3D::UnitZ()));
	return retval;
}

/**Normalize volume defined by in to volume defined by out.
 *
 * This is intended for creating transforms from one viewport to another, i.e.
 * the two boxes should be aligned and differ only in translation + scaling.
 */
Transform3D createTransformNormalize(const DoubleBoundingBox3D& in, const DoubleBoundingBox3D& out)
{
	// translate input bottomleft to origin, scale, translate back to output bottomleft.
	Transform3D T0 = createTransformTranslate(-in.bottomLeft());
	Vector3D inrange = in.range();
	Vector3D outrange = out.range();
	Vector3D scale;

	// check for zero input dimensions
	for (unsigned i = 0; i < scale.size(); ++i)
	{
		if (fabs(inrange[i]) < 1.0E-5)
			scale[i] = 0;
		else
			scale[i] = outrange[i] / inrange[i];
	}
	Transform3D S = createTransformScale(scale);
	Transform3D T1 = createTransformTranslate(out.bottomLeft());
	Transform3D M = T1 * S * T0;
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
	Transform3D t = Transform3D::Identity();
	t.matrix().col(0).head(3) = ivec;
	t.matrix().col(1).head(3) = jvec;
	t.matrix().col(2).head(3) = cross(ivec, jvec);
	t.matrix().col(3).head(3) = center;
	return t;
}

} // namespace ssc
// --------------------------------------------------------

