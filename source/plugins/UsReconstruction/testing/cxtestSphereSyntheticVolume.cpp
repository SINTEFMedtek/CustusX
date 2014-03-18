// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxtestSphereSyntheticVolume.h"
#include "cxTypeConversions.h"

namespace cxtest {

SphereSyntheticVolume::SphereSyntheticVolume(cx::Vector3D dimensions) :
	cx::cxSyntheticVolume(dimensions)
{
	mCenter = mBounds/2;
	mRadius = 0.6 * mBounds.array().minCoeff()/2;
}

void SphereSyntheticVolume::printInfo() const
{
	std::cout << QString("Volume: Sphere, bounds=[%1], C=[%2], R=[%3]")
				 .arg(qstring_cast(mBounds))
				 .arg(qstring_cast(mCenter))
				 .arg(mRadius)
				 << std::endl;
}

void SphereSyntheticVolume::setSphere(const cx::Vector3D& center, double radius)
{
	mCenter = center;
	mRadius = radius;
}

unsigned char
SphereSyntheticVolume::evaluate(const cx::Vector3D &p) const
{
	if ((p - mCenter).norm() < mRadius)
	{
		return 255;
	}
	else
	{
		return 0;
	}
}

}
