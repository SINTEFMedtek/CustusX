/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

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
