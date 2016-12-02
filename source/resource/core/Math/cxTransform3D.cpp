/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxTransform3D.h"

#include <sstream>
#include <vector>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include "cxTypeConversions.h"
#include "cxBoundingBox3D.h"
#include "vtkForwardDeclarations.h"

// --------------------------------------------------------
namespace cx_transform3D_internal
{

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

void fill(Eigen::Affine3d* self, float m[4][4])
{
    if (!m)
        return;
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            (*self)(r, c) = m[r][c];
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

vtkTransformPtr getVtkTransform(const Eigen::Affine3d* self)
{
	vtkTransformPtr retval = vtkTransform::New();
	retval->SetMatrix(self->getVtkMatrix());
	retval->Update();
	return retval;
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

} // namespace cx_transform3D_internal

namespace cx
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

DoubleBoundingBox3D transform(const Transform3D& m, const DoubleBoundingBox3D& bb)
{
	Vector3D a = m.coord(bb.bottomLeft());
	Vector3D b = m.coord(bb.topRight());
	return DoubleBoundingBox3D(a, b);
}

Transform3D createTransformScale(const Vector3D& scale_)
{
	Transform3D retval = Transform3D::Identity();
	retval.scale(scale_);
	return retval;
}

Transform3D createTransformTranslate(const Vector3D& translation)
{
	Transform3D retval = Transform3D::Identity();
	retval.translate(translation);
	return retval;
}

Transform3D createTransformRotateX(const double angle)
{
	Transform3D retval = Transform3D::Identity();
	retval.rotate(Eigen::AngleAxisd(angle, Vector3D::UnitX()));
	return retval;
}

Transform3D createTransformRotateY(const double angle)
{
	Transform3D retval = Transform3D::Identity();
	retval.rotate(Eigen::AngleAxisd(angle, Vector3D::UnitY()));
	return retval;
}

Transform3D createTransformRotateZ(const double angle)
{
	Transform3D retval = Transform3D::Identity();
	retval.rotate(Eigen::AngleAxisd(angle, Vector3D::UnitZ()));
	return retval;
}

Transform3D createTransformRotationBetweenVectors(Vector3D from, Vector3D to)
{
	Vector3D k = cross(from, to);


	// handle special cases
	if (similar(k.length(), 0.0))
	{
		// dot==1 -> point in the same direction
		if (similar(dot(from, to), 1.0))
		{
			return Transform3D::Identity();
		}

		// dot==-1 ->point in opposite directions, cross product will fail.
		// Find an arbitrary vector perpendicular to from, rotate 180 around that one.
		if (similar(dot(from, to), -1.0))
		{
			Vector3D e_x = Vector3D::UnitX();
			Vector3D kk = cross(from, e_x);
			if (similar(kk.length(), 0.0))
			{
				Vector3D e_y = Vector3D::UnitY();
				kk = cross(from, e_y);
			}

			Transform3D retval = Transform3D::Identity();
			retval.rotate(Eigen::AngleAxisd(M_PI, kk));
			return retval;
		}
	}

	double dotnormal = dot(from, to)/from.length()/to.length();
	double angle = acos(dotnormal);

	Transform3D retval = Transform3D::Identity();
	retval.rotate(Eigen::AngleAxisd(angle, k.normal()));
	return retval;
}


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

Transform3D createTransformIJC(const Vector3D& ivec, const Vector3D& jvec, const Vector3D& center)
{
	Transform3D t = Transform3D::Identity();
	t.matrix().col(0).head(3) = ivec;
	t.matrix().col(1).head(3) = jvec;
	t.matrix().col(2).head(3) = cross(ivec, jvec);
	t.matrix().col(3).head(3) = center;
	return t;
}


cxResource_EXPORT Transform3D createTransformLPS2RAS()
{
	return createTransformRotateZ(M_PI);
}

cxResource_EXPORT Transform3D createTransformFromReferenceToExternal(PATIENT_COORDINATE_SYSTEM external)
{
	if (external==pcsRAS)
		return createTransformLPS2RAS();
	else
		return Transform3D::Identity();
}



} // namespace cx
// --------------------------------------------------------

