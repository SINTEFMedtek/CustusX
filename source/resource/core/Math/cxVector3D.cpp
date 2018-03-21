/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxVector3D.h"
#include <vtkMath.h>
#include <vtkSmartPointer.h>
#include "cxUtilHelpers.h"
#include "cxTypeConversions.h"
#include "vtkForwardDeclarations.h"
#include <cmath>
#include <QString>

// --------------------------------------------------------
namespace cx
{

// --------------------------------------------------------
bool similar(double a, double b, double tol)
{
	return fabs(b - a) < tol;
}

Vector3D multiply_elems(const Vector3D& a, const Vector3D& b)
{
	return a.array() * b.array();
}

Vector3D divide_elems(const Vector3D& a, const Vector3D& b)
{
	return a.array() / b.array();
}

Vector3D cross(const Vector3D& a, const Vector3D& b)
{
	return a.cross(b);
}

double dot(const Vector3D& a, const Vector3D& b)
{
	return a.dot(b);
}

bool similar(const Vector3D& a, const Vector3D& b, double tol)
{
	return (b - a).length() < tol;
}

Vector3D unitVector(double thetaXY, double thetaZ)
{
	Vector3D e;
	e[0] = cos(thetaXY) * cos(thetaZ);
	e[1] = sin(thetaXY) * cos(thetaZ);
	e[2] = sin(thetaZ);
	return e;
}

double getThetaXY(Vector3D k)
{
	return atan2(k[1], k[0]);
}

double getThetaZ(Vector3D k)
{
	return atan2(k[2], sqrt(k[0] * k[0] + k[1] * k[1]));
}

Vector3D round(const Vector3D& a)
{
	Vector3D retval;
	for (int i = 0; i < 3; ++i)
		retval[i] = (int) (a[i] + 0.5);

	return retval;
}

Vector3D ceil(const Vector3D& a)
{
	Vector3D retval;
	for (int i = 0; i < 3; ++i)
		retval[i] = std::ceil(a[i]);

	return retval;
}

bool similar(const Eigen::Array3i& a, const Eigen::Array3i& b)
{
	return (b - a).abs().maxCoeff() < 10E-6;;
}

QString prettyFormat(Vector3D val, int decimals, int fieldWidth)
{
	return QString("%1 %2 %3")
			.arg(val[0], fieldWidth, 'f', decimals)
			.arg(val[1], fieldWidth, 'f', decimals)
			.arg(val[2], fieldWidth, 'f', decimals);
}

Eigen::Vector2d fromString(const QString& text)
{
	std::vector<double> raw = convertQString2DoubleVector(text);
	if (raw.size() != 2)
		return Eigen::Vector2d(0, 0);
	return Eigen::Vector2d((double*) &(*raw.begin()));
}

} // namespace cx
// --------------------------------------------------------
