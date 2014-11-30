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
