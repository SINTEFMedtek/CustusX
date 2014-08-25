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
#include "cxAxisConnector.h"

#include "cxPointMetric.h"
#include "cxAxesRep.h"
#include "cxTool.h"
#include "cxSpaceListener.h"
#include "cxSpaceProvider.h"

namespace cx
{

AxisConnector::AxisConnector(CoordinateSystem space, SpaceProviderPtr spaceProvider)
{
	mSpaceProvider = spaceProvider;
	mListener = mSpaceProvider->createListener();
	mListener->setSpace(space);
	connect(mListener.get(), SIGNAL(changed()), this, SLOT(changedSlot()));

	mRep = AxesRep::New(space.toString() + "_axis");
	mRep->setCaption(space.toString(), Vector3D(1, 0, 0));
	mRep->setShowAxesLabels(false);
	mRep->setFontSize(0.08);
	mRep->setAxisLength(0.03);
	this->changedSlot();
}

void AxisConnector::mergeWith(SpaceListenerPtr base)
{
	mBase = base;
	connect(mBase.get(), SIGNAL(changed()), this, SLOT(changedSlot()));
	this->changedSlot();
}

void AxisConnector::connectTo(ToolPtr tool)
{
	mTool = tool;
	connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(changedSlot()));
	this->changedSlot();
}

void AxisConnector::changedSlot()
{
	Transform3D  rMs = mSpaceProvider->get_toMfrom(mListener->getSpace(), CoordinateSystem(csREF));
	mRep->setTransform(rMs);

	mRep->setVisible(true);

	// if connected to tool: check visibility
	if (mTool)
		mRep->setVisible(mTool->getVisible());

	// Dont show if equal to base
	if (mBase)
	{
		Transform3D rMb = mSpaceProvider->get_toMfrom(mBase->getSpace(), CoordinateSystem(csREF));
		if (similar(rMb, rMs))
			mRep->setVisible(false);
	}

}


} // namespace cx

