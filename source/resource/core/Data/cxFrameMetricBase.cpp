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

#include "cxFrameMetricBase.h"
#include "cxBoundingBox3D.h"
#include "cxTypeConversions.h"
#include "cxSpaceProvider.h"
#include "cxSpaceListener.h"

namespace cx {

FrameMetricBase::FrameMetricBase(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
		DataMetric(uid, name, dataManager, spaceProvider),
		mSpace(CoordinateSystem::reference()),
		mFrame(Transform3D::Identity())
{
	mSpaceListener = mSpaceProvider->createListener();
	mSpaceListener->setSpace(mSpace);
//	mSpaceListener.reset(new SpaceListener(mSpace));
	connect(mSpaceListener.get(), SIGNAL(changed()), this, SIGNAL(transformChanged()));
}

FrameMetricBase::~FrameMetricBase()
{
}

QString FrameMetricBase::getParentSpace()
{
	return mSpaceProvider->convertToSpecific(mSpace).mRefObject;
}

void FrameMetricBase::setFrame(const Transform3D& rMt)
{
	mFrame = rMt;
	emit transformChanged();
}

Transform3D FrameMetricBase::getFrame()
{
	return mFrame;
}

Vector3D FrameMetricBase::getCoordinate() const
{
	Vector3D point_t = Vector3D(0,0,0);
	return mFrame.coord(point_t);
}

/** return frame described in ref space F * sMr
  */
Transform3D FrameMetricBase::getRefFrame() const
{
	Transform3D rMq = mSpaceProvider->get_toMfrom(this->getSpace(), CoordinateSystem(csREF));
	return rMq * mFrame;
}

/** return frame described in ref space F * sMr
  */
Vector3D FrameMetricBase::getRefCoord() const
{
	Transform3D rMq = this->getRefFrame();
	Vector3D p_r = rMq.coord(Vector3D(0,0,0));
	return p_r;
}

void FrameMetricBase::setSpace(CoordinateSystem space)
{
	if (space == mSpace)
		return;

	// keep the absolute position (in ref) constant when changing space.
	Transform3D new_M_old = mSpaceProvider->get_toMfrom(this->getSpace(), space);
	mFrame = new_M_old*mFrame;

	mSpace = space;
	mSpaceListener->setSpace(space);
}

CoordinateSystem FrameMetricBase::getSpace() const
{
	return mSpace;
}

DoubleBoundingBox3D FrameMetricBase::boundingBox() const
{
	// convert both inputs to r space
	Transform3D rM0 = mSpaceProvider->get_toMfrom(this->getSpace(), CoordinateSystem(csREF));
	Vector3D p0_r = rM0.coord(this->getCoordinate());

	return DoubleBoundingBox3D(p0_r, p0_r);
}

QString FrameMetricBase::matrixAsSingleLineString() const
{
	std::stringstream stream;
	mFrame.put(stream, 0, ' ');
	return qstring_cast(stream.str());
}

Transform3D FrameMetricBase::getMatrixFromSingleLineString(QStringList& parameterList, int indexWhereMatrixStarts)
{
	QString matrix;
	for(int j = indexWhereMatrixStarts; j < parameterList.size(); ++j)
	{
		matrix += parameterList.at(j);
		matrix += " ";
	}

	return Transform3D::fromString(matrix);
}


} //namespace cx
