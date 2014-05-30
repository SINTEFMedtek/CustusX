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
#include "cxSimulatedImageStreamerService.h"

#include "cxToolManager.h"
#include "cxDataManager.h"
#include "cxReporter.h"
#include "cxSimulateUSWidget.h"

namespace cx
{

SimulatedImageStreamerService::SimulatedImageStreamerService()
{
}

QString SimulatedImageStreamerService::getName()
{
	return "SimulatedImageStreamerService";
}

void SimulatedImageStreamerService::setBackend(VideoServiceBackendPtr backend)
{
	mBackend = backend;
}

void SimulatedImageStreamerService::setImageToStream(QString imageUid)
{
	mImageUidToSimulate = imageUid;
}

void SimulatedImageStreamerService::setGain(double gain)
{
	//must mutex mStreamer
	if(mStreamer)
		mStreamer->setGain(gain);
}

StreamerPtr SimulatedImageStreamerService::createStreamer()
{
	mStreamer.reset(new SimulatedImageStreamer());
	if(!mBackend)
	{
		reporter()->sendError("SimulatedImageStreamerInterface got no VideoServiceBackend");
		return mStreamer;
	}

	ToolPtr tool = mBackend->getToolManager()->findFirstProbe();
	if(!tool)
		reporter()->sendDebug("No tool");
	ImagePtr image = mBackend->getDataManager()->getImage(mImageUidToSimulate);
	if(!image)
		reporter()->sendDebug("No image with uid: "+mImageUidToSimulate);

	mStreamer->initialize(image, tool, mBackend->getDataManager());

	return mStreamer;
}

BaseWidget* SimulatedImageStreamerService::createWidget()
{
	SimulateUSWidget* mSimulationWidget = new SimulateUSWidget();
	return mSimulationWidget;
}

} //end namespace cx
