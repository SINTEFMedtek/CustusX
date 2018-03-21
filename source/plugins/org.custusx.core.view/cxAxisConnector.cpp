/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

