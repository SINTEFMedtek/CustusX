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
#include "cxAxisConnector.h"

#include "sscPointMetric.h"
#include "sscAxesRep.h"
#include "sscTool.h"
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

